// get request on given url
function requestApi(url) {
    return new Promise(function (resolve, reject) {
        var xhr = new XMLHttpRequest();
        xhr.onreadystatechange = function () {
            if (this.readyState == 4) {
                if (this.status == 200) {
                    resolve(JSON.parse(this.responseText));
                } else {
                    reject(xhr.statusText);
                }
            }
        };
        xhr.open("GET", url, true);
        xhr.send();
    });
}

function refreshMovieList(jsonData) {
    //if (jsonData == undefined) return;
    
    movieList = document.getElementById("movieList");
    movieList.innerHTML = "";

    for (let i = 0; i < 10; i++) {
        let url = "https://api.themoviedb.org/3/movie/55"+i+"?api_key=39556c17fbe9e58c430f6a811f19fb1c";
        requestApi(url)
        .then(function (data) {
            //console.log(data);
            //movieList.innerHTML += createCard(data.id, data.original_title, "https://image.tmdb.org/t/p/w500" + data.poster_path, data.overview);
            movieList.appendChild(createCard(data.id, data.original_title, "https://image.tmdb.org/t/p/w500" + data.poster_path, data.overview));
            movieList.ins
        }).catch(function (reason) {
            console.log(url);
            console.error(reason);
            movieList.appendChild(createCard(0, "Error API", "img/missing_poster.png", reason, "error api"));
        });
    }
    setEventListeners();
}

function createCard(id, title, poster, description, type){
    let card = document.createElement("a");
    card.classList.add("card");
    card.classList.add("modalButton");
    card.setAttribute("id", "card_"+id);
        let cardPoster = document.createElement("img");
        cardPoster.classList.add("cardPoster");
        cardPoster.setAttribute("alt", "movie poster");
        cardPoster.setAttribute("src", poster);
        console.log(cardPoster);
        let cardTitle = document.createElement("div");
        cardTitle.classList.add("cardTitle");
        cardTitle.innerText = title;
    //card.appendChild(cardPoster);
    card.appendChild(cardTitle);

    let modalDiv = document.createElement("div");
    modalDiv.classList.add("modalDiv");
    modalDiv.setAttribute("id", "card_"+id+"_modal");
        let modalContent = document.createElement("div");
        modalContent.classList.add("modalContent");
            let mil = document.createElement("div");
            mil.classList.add("movieInfo");
            mil.classList.add("left");
            mil.appendChild(cardPoster);
            let mir = document.createElement("div");
            mir.classList.add("movieInfo");
            mir.classList.add("right");
                let miTitle = document.createElement("h2");
                miTitle.innerText = title;
                let miDescr = document.createElement("p");
                miDescr.innerText = description;
            mir.appendChild(miTitle);
            mir.appendChild(miDescr);
            let play = document.createElement("a");
            play.classList.add("playMovie");
            //TODO play button for movies
            play.setAttribute("id", "movie_file_name");
            play.setAttribute("href", "#");
            play.innerText = "play the movie";
        modalContent.appendChild(mil);
        modalContent.appendChild(mir);
        modalContent.appendChild(play);
    modalDiv.appendChild(modalContent);

    let output = document.createDocumentFragment();
    output.append(card);
    output.append(modalDiv);
    return output;
    /*return `<a href="#" class="card modalButton" id="card_`+id+`">
                <img src="`+poster+`" alt="movie poster" class="moviePoster">
                <div class="movieTitle">`+title+`</div>
            </a>
            <div class="modalDiv" id="card_`+id+`_modal">
                <div class="modalContent">
                    <div class="movieInfo left">
                        <img src="`+poster+`" alt="movie poster" class="">
                    </div>
                    <div class="movieInfo right">
                        <h2>`+title+`</h2>
                        <p>`+description+`</p>
                    </div>
                    <a href="#" class="playMovie" id="movie_file_name">play the movie</a>
                </div>
            </div>`;*/
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
/*window.onload = function(){
    refreshMovieList();
};*/