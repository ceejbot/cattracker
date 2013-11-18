var
	P = require('p-promise')
	;

exports.findChannel = function(bt, address)
{
	var deferred = P.defer();

	bt.findSerialPortChannel(address, function(channel)
	{
		deferred.resolve(channel);
	});

	return deferred.promise;
};

exports.connect = function(bt, address, channel)
{
	var deferred = P.defer();

	bt.connect(address, channel, function()
	{
		deferred.resolve();
	}, function()
	{
		deferred.reject(new Error('failed to connect to ' + address + '@' + channel));
	});

	return deferred.promise;
};

exports.write = function(bt, data)
{
	if (!Buffer.isBuffer(data))
		data = new Buffer(data);

	var deferred = P.defer();

	bt.write(data, function(err, written)
	{
		if (err)
			return deferred.reject(err);

		deferred.resolve(written);
	});

	return deferred.promise;
};
