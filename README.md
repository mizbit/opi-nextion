# opi-nextion

This repository is special for the support of the [Nextion HMI Display](https://nextion.tech)'s.

It is currently *work in progress*.

**Hardware interface**

* I2C slave SC16IS740/750/760 <- UART -> Nextion HMI

The `lib-nextion` will contain the framework. The idea is similiar as with [remote configuration framework](http://www.orangepi-dmx.org/orange-pi-dmx512-rdm/remote-configuration); there is a defined simple communication protocol.

The directory `opi_emac_artnet_nextion` will contain a dummy Art-Net 4 Node for testing `lib-nextion`.

I invite you to participate in this project:

1. Open an Issue
2. Create a directory in `users`, put in your .HMI .zi files and submit a [pull request](https://help.github.com/en/articles/about-pull-requests).

**The protocol**.


* No *Send Component ID*.
* When a page must retrieve data then the '?' must be prefixed to the configuration file name -> Initialize Event.
* When page data must be stored then the '!' must be prefixed to the configuration file name -> Touch Event.
* The startup page, the main page, must just send 'main -> Initialize Event.
* There are well defined [object names](/objects) per page.

Global commands (`prints "",0`)

	!reboot##
	!display#


Example main page.
</br>
![Example main page](images/main.png)

Example Network page.
</br>
![Example main page](images/get.png)
</br>
![Example main page](images/save.png)

http://www.orangepi-dmx.org/
