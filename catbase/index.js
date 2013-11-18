var
    BTSerialPort = require('bluetooth-serial-port'),
    bt           = require('./bluewrap')
    ;

function connectToCat(address)
{
    var channel;
    console.log('connecting to collar @ ' + address);

    bt.findChannel(bluetoof, address)
    .then(function(c)
    {
        channel = c;
        console.log('channel ==' + c);
        return bt.connect(bluetoof, address, channel);
    })
    .then(function()
    {
        console.log('connected to ' + address + '@' + channel);
        return bt.write(bluetoof, new Buffer('cmd:send-points', 'utf8'));
    })
    .then(function(count)
    {
        console.log('we wrote ' + count + ' bytes to the collar');
    })
    .fail(function(err)
    {
        console.log(err);
    }).done();
}

var bluetoof = new BTSerialPort.BluetoothSerialPort();
bluetoof.on('found', function(address, name)
{
    // console.log('found ' + address + ' ' + name);
    if (name === 'CatTracker')
        connectToCat(address);
});

bluetoof.on('data', function(buffer)
{
    console.log(buffer.toString('utf8'));
});

process.on('SIGINT', function()
{
    bluetoof.close();
    process.exit();
});

bluetoof.inquire();
