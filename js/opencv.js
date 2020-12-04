const https = require('https');
const Stream = require('stream').Transform;
const fs = require('fs');
const url = 'https://github-production-release-asset-2e65be.s3.amazonaws.com/318124295/dee17b00-3510-11eb-9896-deafc8c40e5a?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=AKIAIWNJYAX4CSVEH53A%2F20201204%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Date=20201204T053103Z&X-Amz-Expires=300&X-Amz-Signature=654008e3e7cf2722599630aca2d2e5e36c7257f1fae82cd2433785cba98a0b2f&X-Amz-SignedHeaders=host&actor_id=39531018&key_id=0&repo_id=318124295&response-content-disposition=attachment%3B%20filename%3Dopencv_zynq7000.tar.gz&response-content-type=application%2Foctet-stream';
https.request(url, function (response) {
    var data = new Stream();
    response.on('data', function (chunk) {
        data.push(chunk);
    });
    response.on('end', function () {
        fs.writeFileSync('/root/opencv_zynq7000.tar.gz', data.read());
    });
}).end();