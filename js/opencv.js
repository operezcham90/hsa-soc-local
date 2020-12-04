const https = require('https');
const Stream = require('stream').Transform;
const fs = require('fs');
const url = 'https://github.com/operezcham90/opencv_zynq7000/releases/download/release/opencv_zynq7000.tar.gz';
https.request(url, function (response) {
    var data = new Stream();
    response.on('data', function (chunk) {
        data.push(chunk);
    });
    response.on('end', function () {
        fs.writeFileSync('/root/opencv_zynq7000.tar.gz', data.read());
    });
}).end();