service-poke
============

service-poke is a client/server based system to allow extremely lightweight
starting and management of services without imposing much system overhead.
Its intended to be used for synchronizing type of services but could be used
for other things.

Definition: poke - A process that runs a service if it is not already
                   running. If it is already running, schedule it to rerun
                   directly after the current run. If there is already a
                   pending run of that service scheduled, don't schedule yet
                   another one.

Note: It is the last property, not executing too many extra runs that makes it
      so simple and powerful to use.

Poking can be useful for many things, for example:
 * To have a shell script that wrote something important to disk kick off a
   backup to tape run
 * To have a web service that calls a program to sync a backing database
   without allowing users to waste resources by executing too many syncs
 * In a svn commit handler, rsync data to multiple servers while not worrying
   about bursts of commit activity
 
Normally, you could just put the service in cron and play a balancing act with
having the service run frequently enough to not be a problem but infrequently
enough so that it doesn't consume a lot of resources needlessly. This is can be
tricky and often a good balance can't be reached.

With service-poke, the you have more flexibility. You can poke the service from
within scripts that want things done sooner then cron normally would, and set
cron to poke at a significantly lower interval saving resources.


For install instructions, see the INSTALL file.

For usage, see man 8 service-poke and man 8 service-poke-server.
