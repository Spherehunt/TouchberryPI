# TouchBerry Thumper Ruby Commander

This script is able to detect changes in files in the directory '/tmp/touch/' and
send a REST request to the Thumper based on the content of the file inside the dir.

The script should be run on the Raspberry Pi that has the C++ i2c master application
for the QT1070 running and a TouchBerry shield connected to it.

## Requirements

You may need to install some packages

```shell
sudo apt-get install ruby ruby-dev libssl-dev bundler
```

Next do a `bundle install` to install all the required gems.

## Launching the script

First make sure to create a directory `/tmp/touch` with a single file in it (name of file does not matter).

Run the script by issuing

```shell
./thumper_touch.rb
```

You can test by echoing commands to the file as for example:

```shell
echo "strobe" > /tmp/touch/thumper
```
