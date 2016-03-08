var fs = require('fs')

fs.readdir(process.argv[2], function callback (err, list) {
	if (!err){
		for (var i = 0; i < list.length; i++){
			if (list[i].toString().split('.')[1] == process.argv[3]){
				console.log(list[i])
			}
		}
	}	
})
