all: weather

weather: weather.c
	$(CC) -Wall -Wpedantic -Wextra -Os -std=c99 -o $@ $^

clean:
	rm -f weather
