# About the project

Are you fed up with constantly failing in raising your pet mushrooms? Or are you going crazy with yearly losses caused by improper conditions in your industrial plantation? Fret not! We are happy to present you semiautomatic mushroom plantation monitoring system - *PIECZARKA*

Pieczarka has been developed during Internet of Things course at University of Warsaw in 2018/2019 by:
* [Adam Wiktor](@thewizytory)
* [Julia Bazińska](@lamyiowce)
* [Kamil Mykitiuk](@Kamyki)
* [Tomasz Kanas](@Kanadas)

# Features
You can easily deploy network of energy efficient nodes monitoring air temperature and humidity, soil moisture and growth progress. Thanks to efficient usage of radio communication, number of nodes scale up easily. You can also start many independent plantations and monitor theirs progress in our stylish web client. 

# Used technologies

* [CoAP](http://coap.technology/) communication protocol
* [aiocoap](https://github.com/chrysn/aiocoap) lightweigh asynchronus coap server written in python
* [mbed](https://www.mbed.com/en/platform/mbed-os/) open source embedded operating system

# Infrastructure

* 3 sensor nodes – each one is collecting and sending data from their sensors, their refresh rate can be changed from our web client
* gateway node – is collecting data from sensor nodes over radio and sends it to the server via internet connection
* server – saves data database and gives REST API for our web client
* web client – gives you access to all collected data and lets you change the refresh rate of each sensor node separately
