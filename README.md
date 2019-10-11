# opi-nextion

This repository is special for the support of the [Nextion HMI Display](https://nextion.tech)'s.

It is currently *work in progress*.

The `lib-nextion` will contain the framework. The idea is the same as with remote configuration framework; there is a defined simple communication protocol.

I invite you to participate in this project:

1. Open an Issue
2. Create a directory in `users`, put in your .tft file and submit a [pull request](https://help.github.com/en/articles/about-pull-requests).

**The protocol**.

* No events must be generated.
* When a page must retrieve data then the '?' must be prefixed to the configuration file name.
* Whan page data must be stored then the '!' must be prefixed to the configuration file name.
* The startup page, the main page, must just send 'main'

Example main page.
![Example main page](images/main.png)
<img src="https://raw.githubusercontent.com/vanvught/opi-nextion/master/images/main.png" />


Example Network page.


http://www.orangepi-dmx.org/
