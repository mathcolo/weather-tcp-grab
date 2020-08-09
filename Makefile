all: weather

weather: weather.c
	$(CC) -Os -std=c99 -o $@ $^

clean:
	rm -f weather
