function multiAnswer(answer) {
    switch(answer) {
        case '1_1':
            window.open("/ni.html", "_self");
            break;
        case '2_2':
            window.open("/foe.html", "_self");
            break;
        case '3_1':
            window.open("/success.html", "_self");
            break;
        default:
            window.open("/bridge.html","_self");
    }
}
