var fs = require('fs')

fs.readFile(process.argv[2], 'utf8', function callback (err, str) {
	if (!err){
		var newlineCount = str.split('\n').length - 1
		console.log(newlineCount)
	}	
})

