function addMovies(moviesJson) {
    movieList = document.getElementById("movieList");

    for (let i = 0; i < 10; i++) {
        let poster = "img/missing_poster.png";
        let movieTitle = "Movie Title "+i;
        let movieDescription = "Movie Description";
        
        
        movieList.innerHTML +=
        `<a href="#" class="card modalButton" id="card_`+i+`">
            <img src="`+poster+`" alt="movie poster" class="moviePoster">
            <div class="movieTitle">`+movieTitle+`</div>
        </a>
        <div class="modalDiv" id="card_`+i+`_modal">
            <div class="modalContent">
                <div class="movieInfo left">
                    <img src="`+poster+`" alt="movie poster" class="">
                </div>
                <div class="movieInfo right">
                    <h2>`+movieTitle+`</h2>
                    <p>`+movieDescription+`</p>
                </div>
                <a href="#" class="playMovie" id="movie_file_name">play the movie</a>
            </div>
        </div>`;
    }
}

function setEventListeners(){

    //Modal div control
    var modalButtons = document.getElementsByClassName("modalButton");
    for (let modalButton of modalButtons) {
        modalButton.addEventListener("click", function (event) {
            document.getElementById(modalButton.attributes["id"].value + "_modal").style.display = "block";
        });
    }
    window.addEventListener("click", function(event){
        if(event.target.classList.contains("modalDiv")) event.target.style.display = "none";
    });

    var anchors = document.getElementsByTagName("a");
    for (let anchor of anchors) {
        anchor.addEventListener("click", function(event){
            event.preventDefault()
        });
        if (anchor.classList.contains("weblink")) {
            anchor.addEventListener("click", function(event) {
                console.log(this.attributes["href"].value);
                //TODO call c++ function to launch webbrowser to url value
            })
        }
    }
}
window.onload = function(){
    addMovies();
    setEventListeners();
};