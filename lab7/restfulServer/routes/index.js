var express = require('express');
var fs = require('fs');
var router = express.Router();


/* GET home page. */
router.get('/', function(req, res, next) {
  res.sendFile('weather.html', { root: 'public' });
});

router.get('/getcity', function(req, res, next){
	//console.log("In getcity route")

	fs.readFile(__dirname + '/cities.dat.txt', function(err, data){
		if(err) throw err;

		var myRe = new RegExp("^" + req.query.q);
		//console.log(myRe);

		var jsonresult = [];
		var cities = data.toString().split("\n");
		for(var i = 0;  i < cities.length; i++) {
			var result = cities[i].search(myRe);
			if(result != -1){
				//console.log(cities[i]);
				jsonresult.push({city:cities[i]});
			}
		}
		//console.log(jsonresult);
		res.status(200).json(jsonresult);
		

	})
});

router.get('/quote', function(req, res, next){
	fs.readFile(__dirname + '/quotes.dat.txt', function(err, data){
		if(err) throw err;
			console.log("in quote");

			var jsonresult = [];
			var quotes = data.toString().split("\n");
			var length = quotes.length;
			var randomInt = Math.round(Math.random()*(length-1));
			jsonresult.push({quote:quotes[randomInt]});
			res.status(200).json(jsonresult);

	});
});


module.exports = router;
