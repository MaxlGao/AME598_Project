var MS = require("mongoskin");
var express = require("express");
var app = express();
var bodyParser = require('body-parser');
var errorHandler = require('errorhandler');
var methodOverride = require('method-override');
var hostname = process.env.HOSTNAME || 'localhost';
var port = 8080;
var VALUEx = "";
var VALUEy = "";
var VALUEtime = 0;
var VALUEimage = "";

var db = MS.db("mongodb://localhost:27017/cameraData");

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true })); // for parsing application/x-www-form-urlencoded
app.use(methodOverride());
app.use(express.static(__dirname + '/public'));
app.use(errorHandler());

app.get("/", function (req, res) {
    res.redirect("/index.html");
});

app.get("/getLatest", function (req, res) {
  db.collection("dataCamera").find({}).sort({time:-1}).limit(10).toArray(function(err, result){
    res.send(JSON.stringify(result));
  });
});

app.get("/getData", function (req, res) {
  var from = parseInt(req.query.from);
  var to = parseInt(req.query.to);
  db.collection("dataCamera").find({time:{$gt:from, $lt:to}}).sort({time:-1}).toArray(function(err, result){
    res.send(JSON.stringify(result));
  });
});


app.get("/getValue", function (req, res) {
  //res.writeHead(200, {'Content-Type': 'text/plain'});
  res.send(VALUEx + " " + VALUEy + " " + VALUEtime + " " + VALUEimage + "\r");
});

app.post("/setValue", function (req, res) {
  // Access data from the body of the request
  VALUEx = req.body.x;
  VALUEy = req.body.y;
  VALUEtime = new Date().getTime();
  VALUEimage = req.body.image;

  var dataObj = {
      x: VALUEx,
      y: VALUEy,
      time: VALUEtime,
      image: VALUEimage
  };

  db.collection("dataCamera").insertOne(dataObj, function(err, result) {
      if (err) {
          console.error("Error adding data:", err);
          res.status(500).send("Error inserting data");
      } else {
          console.log("Added data:", JSON.stringify(dataObj));
          res.send(JSON.stringify({ time: VALUEtime }));
      }
  });
});

console.log("Simple static server listening at http://" + hostname + ":" + port);
app.listen(port);
