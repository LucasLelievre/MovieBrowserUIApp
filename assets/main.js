function setEventListeners(){
    var cards = document.getElementsByClassName("card");
    for (let card of cards) {
        card.addEventListener("click", function(event) {
            console.log(this);
        });
    }
}