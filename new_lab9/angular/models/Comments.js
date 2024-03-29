var mongoose = require('mongoose');
var CommentSchema = new mongoose.Schema({
  title: String,
  upvotes: {type: Number, default: 0},
  color: String,
});

CommentSchema.methods.upvote = function(cb) {
  this.upvotes += 1;
  this.save(cb);
};

mongoose.model('Comment', CommentSchema);

