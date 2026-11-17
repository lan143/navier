# Navier OTA deployment orchestration.
#
# OTA credentials live in the untracked .env file (see .env.example).
# scripts/ota_env.py feeds NAVIER_OTA_IP / NAVIER_OTA_AUTH into the
# kc868a2_ota PlatformIO environment, so recipes here only need the IP for
# the device HTTP API. NAVIER_OTA_AUTH is never printed.
#
# PlatformIO exposes both `uploadfs` (protocol-aware) and `uploadfsota`. With
# upload_protocol = espota in env:kc868a2_ota both use espota; this Makefile
# uses the explicit `uploadfsota` target for the LittleFS UI upload.
#
# GNU make 3.81 ignores .SHELLFLAGS, so multi-command recipes start with
# `set -e`; every other recipe line is checked by make itself.

SHELL := /bin/sh

ENV_FILE ?= .env

# Extract KEY from $(ENV_FILE) at make time. Handles CRLF, leading/trailing
# whitespace, comment/blank lines and optional single/double quotes.
define dotenv_get
$(shell if [ -f "$(ENV_FILE)" ]; then \
	tr -d '\r' < "$(ENV_FILE)" | sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$$//' | \
	sed -n 's/^$(1)[[:space:]]*=[[:space:]]*//p' | \
	tail -n 1 | \
	sed -e 's/[[:space:]]*$$//' -e 's/^"//' -e 's/"$$//' -e "s/^'//" -e "s/'\$$//"; \
fi)
endef

NAVIER_OTA_IP := $(call dotenv_get,NAVIER_OTA_IP)
NAVIER_OTA_AUTH := $(call dotenv_get,NAVIER_OTA_AUTH)

# `make help` works before .env exists; every other goal needs the OTA settings.
ifeq ($(filter help,$(MAKECMDGOALS)),)
ifeq ($(wildcard $(ENV_FILE)),)
$(error Missing .env file - copy .env.example to .env and set NAVIER_OTA_IP and NAVIER_OTA_AUTH)
endif
ifeq ($(strip $(NAVIER_OTA_IP)),)
$(error NAVIER_OTA_IP is missing from .env - see .env.example)
endif
ifeq ($(strip $(NAVIER_OTA_AUTH)),)
$(error NAVIER_OTA_AUTH is missing from .env - see .env.example)
endif
endif

PIO ?= pio
HOST ?= http://$(NAVIER_OTA_IP)
WAIT_INITIAL ?= 20
WAIT_TIMEOUT ?= 240
POLL_INTERVAL ?= 5
BACKUP_DIR ?= build/deploy
DEPLOY_TIMESTAMP := $(shell date +%Y%m%d-%H%M%S)
BACKUP_ZIP ?= $(BACKUP_DIR)/navier-config-$(DEPLOY_TIMESTAMP).zip
RESTORE_ZIP ?= $(if $(strip $(ZIP)),$(strip $(ZIP)),$(BACKUP_DIR)/latest.zip)

# LittleFS data slot size (0x160000) and the 80% warning threshold.
DATA_BUDGET_BYTES := 1441792
DATA_WARN_BYTES := 1153433

FIRMWARE_CMD = $(PIO) run -e kc868a2_ota -t upload
UI_CMD = $(PIO) run -e kc868a2_ota -t uploadfsota

# Poll /api/status until the device answers with a valid JSON body.
define wait_for_boot
@set -e; \
deadline=$$(($$(date +%s) + $(WAIT_TIMEOUT))); \
attempt=0; \
while :; do \
	attempt=$$((attempt + 1)); \
	if body=$$(curl -fsS --connect-timeout 3 --max-time 5 "$(HOST)/api/status" 2>/dev/null) && \
		printf '%s' "$$body" | grep -q '"freeHeap"'; then \
		echo "Device online after $$attempt attempt(s)."; \
		printf '%s\n' "$$body"; \
		exit 0; \
	fi; \
	if [ "$$(date +%s)" -ge "$$deadline" ]; then \
		echo "ERROR: device at $(HOST) did not become ready within $(WAIT_TIMEOUT)s (polling every $(POLL_INTERVAL)s)." >&2; \
		exit 1; \
	fi; \
	echo "Waiting for device at $(HOST) (attempt $$attempt)..."; \
	sleep $(POLL_INTERVAL); \
done
endef

# Download the STORE zip, verify it, and refresh the stable latest.zip copy.
define backup_cmd
@set -e; \
mkdir -p "$(BACKUP_DIR)"; \
rm -f "$(BACKUP_ZIP)"; \
if ! curl -fsS -o "$(BACKUP_ZIP)" -D "$(BACKUP_DIR)/backup-headers.txt" "$(HOST)/api/config/backup"; then \
	rm -f "$(BACKUP_ZIP)"; \
	echo "ERROR: device has no configuration files to back up (GET $(HOST)/api/config/backup failed)." >&2; \
	exit 1; \
fi; \
size=$$(wc -c < "$(BACKUP_ZIP)" | tr -d ' '); \
if [ "$$size" -eq 0 ]; then \
	rm -f "$(BACKUP_ZIP)"; \
	echo "ERROR: backup download is empty." >&2; \
	exit 1; \
fi; \
if [ "$$size" -gt 16384 ]; then \
	rm -f "$(BACKUP_ZIP)"; \
	echo "ERROR: backup is $$size bytes, larger than the 16 KB import limit." >&2; \
	exit 1; \
fi; \
if [ "$$(head -c 2 "$(BACKUP_ZIP)")" != "PK" ]; then \
	rm -f "$(BACKUP_ZIP)"; \
	echo "ERROR: backup does not look like a zip file (missing PK magic)." >&2; \
	exit 1; \
fi; \
if [ "$(abspath $(BACKUP_ZIP))" != "$(abspath $(BACKUP_DIR)/latest.zip)" ]; then \
	cp -f "$(BACKUP_ZIP)" "$(BACKUP_DIR)/latest.zip"; \
fi; \
echo "Backup saved to $(BACKUP_ZIP) ($$size bytes); latest copy at $(BACKUP_DIR)/latest.zip."
endef

# POST a backup zip. Capture the status code and response body separately so
# old curl builds without --fail-with-body still get readable errors.
define restore_cmd
@set -e; \
if [ ! -f "$(1)" ]; then \
	echo "ERROR: backup file not found: $(1)" >&2; \
	exit 1; \
fi; \
mkdir -p "$(BACKUP_DIR)"; \
response=$$(mktemp "$(BACKUP_DIR)/restore-response.XXXXXX"); \
status=$$(curl -sS -o "$$response" -w '%{http_code}' --connect-timeout 3 --max-time 30 \
	-X POST -H 'Content-Type: application/octet-stream' \
	--data-binary @"$(1)" "$(HOST)/api/config/backup") || { \
	echo "ERROR: could not POST the backup to $(HOST)/api/config/backup." >&2; \
	if [ -s "$$response" ]; then cat "$$response" >&2; fi; \
	rm -f "$$response"; \
	exit 1; \
}; \
if [ "$$status" != "200" ]; then \
	echo "ERROR: restore failed with HTTP $$status." >&2; \
	if [ -s "$$response" ]; then echo "Device response:" >&2; cat "$$response" >&2; fi; \
	echo "If the firmware changed, the backup may be rejected (config version/CRC mismatch)." >&2; \
	echo "Restore again with 'make restore ZIP=...'." >&2; \
	rm -f "$$response"; \
	exit 1; \
fi; \
if [ -s "$$response" ]; then cat "$$response"; echo; fi; \
rm -f "$$response"
endef

# Warn when data/ approaches the LittleFS slot size; the upload itself is still
# attempted so the device reports the real failure.
define check_data_size
@size=$$(du -sk "$(abspath data)" 2>/dev/null | awk '{print $$1 * 1024}'); \
if [ -n "$$size" ] && [ "$$size" -gt $(DATA_WARN_BYTES) ]; then \
	echo "WARNING: data/ is $$size bytes, over 80%% of the $(DATA_BUDGET_BYTES)-byte LittleFS slot." >&2; \
fi
endef

.PHONY: deploy firmware firmware-ota wait-for-boot backup backup-only ui ui-ota restore restore-only status help

.DEFAULT_GOAL := deploy

# Keep the ordered pipeline in one recipe: recipe lines stay sequential even
# under make -j, while the standalone targets share the same step commands.
deploy:
	@echo "Navier OTA deployment plan:"
	@echo "  1/7 flash firmware over OTA"
	@echo "  2/7 wait for the device to boot"
	@echo "  3/7 back up device configuration via /api/config/backup"
	@echo "  4/7 flash the LittleFS UI over OTA"
	@echo "  5/7 wait for the device to boot after the UI upload"
	@echo "  6/7 restore the configuration backup"
	@echo "  7/7 wait for the reboot and verify /api/status"
	@echo
	@echo "[1/7] Flashing firmware over OTA..."
	$(FIRMWARE_CMD)
	@echo "[2/7] Waiting for the device to boot..."
	@sleep $(WAIT_INITIAL)
	$(wait_for_boot)
	@echo "[3/7] Backing up device configuration..."
	$(backup_cmd)
	@echo "[4/7] Flashing LittleFS UI over OTA..."
	$(check_data_size)
	$(UI_CMD)
	@echo "[5/7] Waiting for the device to boot after the UI upload..."
	@sleep $(WAIT_INITIAL)
	$(wait_for_boot)
	@echo "[6/7] Restoring device configuration..."
	$(call restore_cmd,$(BACKUP_ZIP))
	@echo "[7/7] Waiting for the reboot and verifying device status..."
	@sleep $(WAIT_INITIAL)
	$(wait_for_boot)
	@body=$$(curl -fsS --connect-timeout 3 --max-time 5 "$(HOST)/api/status"); \
		printf '%s\n' "$$body"; \
		printf '%s' "$$body" | grep -q '"freeHeap"' || { echo "ERROR: /api/status did not return freeHeap." >&2; exit 1; }; \
		echo "Deployment complete: firmware, UI and configuration are in sync."

firmware-ota:
	@echo "Flashing firmware over OTA..."
	$(FIRMWARE_CMD)

firmware: firmware-ota

wait-for-boot:
	@echo "Waiting $(WAIT_INITIAL)s for the device to boot..."
	@sleep $(WAIT_INITIAL)
	$(wait_for_boot)

backup:
	@echo "Backing up device configuration to $(BACKUP_ZIP)..."
	$(backup_cmd)

backup-only: backup

ui-ota:
	@echo "Flashing LittleFS UI over OTA..."
	$(check_data_size)
	$(UI_CMD)

ui: ui-ota

restore:
	@echo "Restoring device configuration from $(RESTORE_ZIP)..."
	$(call restore_cmd,$(RESTORE_ZIP))

restore-only:
	@if [ -z "$(strip $(ZIP))" ]; then \
		echo "ERROR: ZIP is required. Usage: make restore-only ZIP=build/deploy/latest.zip" >&2; \
		exit 1; \
	fi
	$(call restore_cmd,$(ZIP))

status:
	@curl -fsS --connect-timeout 3 --max-time 5 "$(HOST)/api/status" || { echo "ERROR: could not read $(HOST)/api/status." >&2; exit 1; }
	@echo

help:
	@echo "Navier OTA deployment"
	@echo ""
	@echo "  make deploy            Full OTA pipeline: firmware -> wait -> backup -> UI -> wait -> restore -> verify"
	@echo "  make backup            Back up device configuration to $(BACKUP_ZIP)"
	@echo "  make restore           Restore the latest backup from $(BACKUP_DIR)/latest.zip"
	@echo "  make status            Show device status from GET /api/status"
	@echo "  make firmware          Flash firmware over OTA only"
	@echo "  make ui                Flash the LittleFS UI over OTA only"
	@echo "  make firmware-ota      Target behind 'make firmware'"
	@echo "  make ui-ota            Target behind 'make ui'"
	@echo "  make wait-for-boot     Wait for the device to answer /api/status"
	@echo "  make backup-only       Same as 'make backup'"
	@echo "  make restore-only      Restore an explicit backup (requires ZIP=...)"
	@echo ""
	@echo "Overridable: HOST, PIO, WAIT_INITIAL, WAIT_TIMEOUT, POLL_INTERVAL, BACKUP_DIR, BACKUP_ZIP, ZIP"
	@echo "Credentials come from .env (copy .env.example). The auth value is never printed."
