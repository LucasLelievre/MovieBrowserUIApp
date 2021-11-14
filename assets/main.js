function setEventListeners(){
    var anchors = document.getElementsByTagName("a");
    for (anchor of anchors) {
        anchor.addEventListener("click", function(event){
            event.preventDefault()
        });
        if (anchor.classList.contains("card")) {
            anchor.addEventListener("click", function(event) {
            console.log(this);
        });
    }
        if (anchor.classList.contains("weblink")) {
            anchor.addEventListener("click", function(event) {
                console.log(this.attributes["href"].value);
                //TODO call c++ function to launch webbrowser to url value
            })
}
    }

    setBut = document.getElementById("settingsButton");
    setPan = document.getElementById("settingsPanel");
    setBut.addEventListener("click", function(event){
        setPan.style.display = "block";
    });
    window.addEventListener("click", function(event){
        if(event.target == setPan) setPan.style.display = "none";
    });
}
