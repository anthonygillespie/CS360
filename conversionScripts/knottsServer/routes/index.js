var express = require('express');
var multer = require('multer')
var upload = multer({ dest:'uploads/'})
var router = express.Router();

/* GET home page. */
router.get('/', function(req, res, next) {
  res.render('index', { title: 'Express' });
});

router.post('/upload', upload.single('Charleee'), function(req, res, next) {
  console.log("upload called")
  console.log(req.body)
  res.status(200).send
});

module.exports = router;
