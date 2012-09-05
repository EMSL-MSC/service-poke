#!/usr/bin/python
import servicepoke
error = servicepoke.poke('/tmp/poke.sock', 'foo')
if error:
	print error
