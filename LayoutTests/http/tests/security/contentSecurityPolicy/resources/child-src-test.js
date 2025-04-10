var EXPECT_BLOCK = true;
var EXPECT_LOAD = false;

window.jsTestIsAsync = true;
window.wasPostTestScriptParsed = true;

var iframe;
function injectFrame(url, shouldBlock) {
    window.onload = function () {
        iframe = document.createElement('iframe');
        iframe.onload = iframeLoaded(shouldBlock);
        iframe.src = url;
        document.body.appendChild(iframe);
    };
}

function iframeLoaded(expectBlock) {
    return function(ev) {
        var failed = true;
        try {
            console.log("IFrame load event fired: the IFrame's location is '" + ev.target.contentWindow.location.href + "'.");
            if (expectBlock) {
                testFailed("The IFrame should have been blocked (or cross-origin). It wasn't.");
                failed = true;
            } else {
                testPassed("The IFrame should not have been blocked. It wasn't.");
                failed = false;
            }
        } catch (ex) {
            debug("IFrame load event fired: the IFrame is cross-origin (or was blocked).");
            if (expectBlock) {
                testPassed("The IFrame should have been blocked (or cross-origin). It was.");
                failed = false;
            } else {
                testFailed("The IFrame should not have been blocked. It was.");
                failed = true;
            }
        }
        finishJSTest();
    };
}

function injectFrameRedirectingTo(url, shouldBlock) {
    injectFrame("/security/contentSecurityPolicy/resources/redir.py?url=" + url, shouldBlock);
}

function injectWorker(url, expectBlock, isAsync) {
    window.onload = function() {
        if (isAsync) {
            try {
                const w = new Worker(url);
                if (expectBlock == EXPECT_BLOCK) {
                    w.onerror = () => {
                        testPassed("The worker failed asynchronously");
                        finishJSTest();
                    }
                }
            } catch(e) {
                testFailed("Constructing a worker failed with " + e);
            }
            return;
        }
        if (expectBlock == EXPECT_BLOCK)
            shouldThrow("var w = new Worker('" + url + "');");
        else
            shouldNotThrow("var w = new Worker('" + url + "');");
        finishJSTest();
    };
}
