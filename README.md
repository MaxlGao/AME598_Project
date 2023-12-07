# User Guide

## Initial

Create EC2 instance on AWS Academy, Ubuntu 20.04.

Configure security settings to 1: SSH traffic to port 22 (default), 2: All TCP traffic to port 8080, 3: TCP traffic to port 21017 from your IP (optional)

Launch instance

## In the instance

curl -s https://gist.githubusercontent.com/tejaswigowda/f289e9bff13d152876e8d4b3281142f8/raw/df37df2e16a3166e686357a045751a7c18bbeebe/installNodeMongoUbuntu16.04.sh | bash

sudo npm install -g n

sudo npm install -g http-server

git clone https://github.com/MaxlGao/AME598_Project.git

sudo service mongod start

sudo nano /etc/mongod.conf

## In mongodb.conf:

Change bindIP to 0.0.0.0

## Next Steps:

If already running: sudo service mongod restart (do if changing security settings) (or if mongocompass isnt behaving)

sudo npm install mongoskin

sudo npm install mongodb@^2.0

cd AME598_Project/server

sudo nano server.js

node server.js

