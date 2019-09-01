// src: http://lea.verou.me/2016/12/resolve-promises-externally-with-this-one-weird-trick/
function defer() 
{
	var res, rej;

	var promise = new Promise((resolve, reject) => {
		res = resolve;
		rej = reject;
	});

	promise.resolve = res;
	promise.reject = rej;

	return promise;
}
