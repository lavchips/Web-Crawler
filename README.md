## Web Crawler ##
##### Last Modified Date: Apr / 9 / 2020 #####
☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆

[Command]

Compilation: make
Execution:   ./crawler [website address]

☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆

Unfortunately, running the code successfully requires unimelb VPN
as well as certain executing environment which might have been deprecated.

A snippet of final submission test results is directly copy-pasted
below from GitLab CI/CD -> Jobs, which could be considered as a form
of verification and validation of the code.

[AGG_RESULT] single-page-minimal, PASS<br>
[AGG_RESULT] one-neighbour-minimal, PASS<br>
[AGG_RESULT] simple-cycle-minimal, PASS<br>
[AGG_RESULT] mirror-minimal, PASS<br>
[AGG_RESULT] fully-connected-minimal, PASS<br>
[AGG_RESULT] fully-connected-basic, PASS<br>
[AGG_RESULT] fully-connected-medium, PASS<br>
[AGG_RESULT] fully-connected-advanced, PASS<br>
[AGG_RESULT] stress-test-advanced, PASS<br>
[AGG_RESULT] sparsely-connected-minimal, PASS<br>
[AGG_RESULT] sparsely-connected-basic, PASS<br>
[AGG_RESULT] sparsely-connected-medium, PASS<br>
[AGG_RESULT] sparsely-connected-advanced, PASS<br>
[AGG_RESULT] simple-branch-minimal, PASS<br>
[AGG_RESULT] many-404-minimal, PASS<br>
[AGG_RESULT] mixed-minimal, PASS<br>
[AGG_RESULT] mixed-basic, PASS<br>
[AGG_RESULT] mixed-medium, PASS<br>
[AGG_RESULT] mixed-advanced, PASS<br>
[AGG_RESULT] single-page-401-minimal, PASS<br>
[FINAL] ALL PASS :) Congratulations!

☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆

In terms of handling different status codes, they will be handled as such:

         200    - accept the response happily

    404/414/410 - will not be parsed

       503/504  - a new connection will be established and the old request will be sent again

         301    - get the url of the redirecting page and enqueue it into the "waiting list" for fetching

         401    - a Authorization header and base64 encoded id:password will be appended to the original request
                  and this new request will be sent in order to get access to page content

☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆
