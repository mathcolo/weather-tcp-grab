weather-tcp-grab
================

weather-tcp-grab is a C program that grabs weather data from the [Davis WeatherLinkIP logger](http://www.davisnet.com/weather/products/weather_product.asp?pnum=06555) over TCP port 22222. It can be modified to output many other elements of the data structure returned from the logger, but at this time, the program only prints the current outside temperature.

If you have come across this page looking for an easy way to use your personal weather station data in your own program, then this is your answer.

## Compilation

This has been tested on Ubuntu 12.04.2 LTS x86 with gcc 4.6.3 installed.

    gcc -Os -std=c99 weather.c -o weather

## Usage

    ./weather <ip>

## License
	
This utility is licensed under version 3 of the GNU LGPL License. For more information, see the "LICENSE" document in the respository or visit [the GNU site directly](http://www.gnu.org/licenses/lgpl.html).

## Acknowledgments

Thanks, [Morgan Jones](https://github.com/integ3r), for effectively making this thing possible, and teaching me that I could cast raw bytes to a packed C struct. Awesome.