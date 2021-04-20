## Volatile Variables

Sometimes the process synchronization functionality provided by Pico is just too expensive.

If a variable is updated only by one core and we just need to detect any changes on the other core - it is good enough to declare the variable as volatile!

