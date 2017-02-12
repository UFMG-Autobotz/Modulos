/* 
 * Randall Schmidt
 * A task scheduler that keeps an array of tasks
 * and uses timer interrupts at regular intervals
 * to execute those tasks at approximately the time
 * specified by the caller. In this implementation
 * the caller must know an upper limit for how many
 * tasks will be scheduled at once. This kind of
 * scheme works best with a small maximum number of
 * scheduled tasks and a low time resolution.
 * Otherwise you may be using more clock cycles
 * than you'd like.
 *
 * Thanks to amandaghassaei @ http://www.instructables.com/id/Arduino-Timer-Interrupts/step2/Structuring-Timer-Interrupts/
 * for the timer setup code.
 */

#ifndef TimerScheduler_h
#define TimerScheduler_h

void setupTaskScheduler(int _maxTasks, unsigned long _tickIntervalMs);
int scheduleTimer1Task(void (*action)(void*), void* argument, unsigned long delayMs);
void startSchedulerTicking();

#endif
