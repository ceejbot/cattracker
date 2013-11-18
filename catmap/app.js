var
	express = require('express'),
	fs      = require('fs'),
	http    = require('http'),
	path    = require('path')
	;

var app = express();

var appname = 'catmapper';
var package = require('./package.json');
app.locals.pretty = true;
// ----------------------------------------------------------------------

// all environments
app.set('port', process.env.PORT || 3000);
app.set('views', __dirname + '/views');
app.set('view engine', 'jade');
app.use(express.favicon());
app.use(express.static(path.join(__dirname, 'public')));
app.use(express.logger({ format: 'dev'}));
app.use(express.json());
app.use(express.urlencoded());
app.use(express.methodOverride());
app.use(express.cookieParser('yeah this needs to be configurable'));
app.use(app.router);

if ('development' == app.get('env'))
{
	app.use(express.errorHandler());
}

// ----------------------------------------------------------------------

app.get('/', function(request, response)
{
	// TODO
});

app.get('/ping', function(request, response)
{
	var health =
	{
		pid:    process.pid,
		uptime: process.uptime(),
		memory: process.memoryUsage(),
	};
	response.json(health);
});

// ----------------------------------------------------------------------

var s = app.listen(app.get('port'), function()
{
	console.log('Express server listening on port ' + app.get('port'));
});
