#pragma once

#include <Arduino.h>
#include <cstdint>
#include <esp_log.h>
#include <esp_timer.h>

// Coalesces "reply then reboot" requests so the HTTP response actually reaches
// the client.
//
// ESPAsyncWebServer 3.x does not write the socket from send()/send(200, ...):
// send() only stores the response, and the real write happens in
// AsyncWebServerRequest::_send() AFTER the request handler returns. A handler
// that does `send(...); delay(N); ESP.restart();` therefore reboots the ESP
// before the response is queued, so the browser never sees the success reply.
//
// The fix: handlers register a deadline with schedule() and return immediately;
// loop() (loopTask, not the AsyncTCP task) calls update() and reboots once the
// grace period has elapsed, by which point the response has flushed.
//
// Cross-task protocol: the writer stores the deadline first, then sets pending.
// The reader (loop task) checks pending before reading the deadline, so it can
// never observe pending == true with a stale/partial deadline.
class RebootAfterResponse
{
public:
    // Request a reboot at least graceMs milliseconds from now. The first
    // schedule() wins: a later call while a reboot is already pending is a
    // no-op, so the earliest deadline is honored. Never blocks.
    static void schedule(uint32_t graceMs)
    {
        if (pending()) {
            return;
        }

        // Store order is the protocol: deadline before pending.
        deadlineUs() = esp_timer_get_time() + (int64_t)graceMs * 1000;
        pending() = true;
    }

    // Reboot once the scheduled grace period has elapsed. Cheap when idle and
    // intended to be the first thing loop() does every cycle. Never blocks.
    static void update()
    {
        if (!pending()) {
            return;
        }

        if (esp_timer_get_time() >= deadlineUs()) {
            pending() = false;
            ESP_LOGI("main", "rebooting after deferred schedule");
            ESP.restart();
        }
    }

private:
    // C++11 has no inline variables, so use Meyers function-local statics:
    // inline member functions guarantee a single instance across TUs while
    // avoiding any non-inline header-level definition.
    static volatile bool& pending()
    {
        static volatile bool p = false;
        return p;
    }

    static volatile int64_t& deadlineUs()
    {
        static volatile int64_t d = 0;
        return d;
    }
};
