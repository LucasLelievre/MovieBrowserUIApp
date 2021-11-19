// get request on given url
function getDataFromURL(url) {
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

function refreshMovieCards(data) {
    movieList = document.getElementById("movieList");
    movieList.innerHTML = "";

    JSON.parse(data).data.forEach(element => {
        console.log(element);
        //TODO dirrent url based on element's type
        let url = "https://api.themoviedb.org/3/search/multi?api_key=39556c17fbe9e58c430f6a811f19fb1c&query="+element.name;
        getDataFromURL(url)
        .then(function (data) {
            let movie = data.results[0];
            console.log(movie);
            movieList.appendChild(createCard(movie.id, movie.original_title, "https://image.tmdb.org/t/p/w500" + movie.poster_path, movie.overview));
        }).catch(function (reason) {
            console.log(url);
            console.error(reason);
            movieList.appendChild(createCard(0, "Error API", "img/missing_poster.png", reason, "error api"));
        });
    });
}

function createCard(id, title, poster, description, type){
    let card = document.createElement("a");
    card.classList.add("card");
    card.classList.add("modalButton");
    card.setAttribute("id", "card_"+id);
        let cardPoster = document.createElement("img");
        cardPoster.classList.add("cardPoster");
        cardPoster.setAttribute("alt", "movie cardPoster");
        cardPoster.setAttribute("src", poster);
        console.log(cardPoster);
        let cardTitle = document.createElement("div");
        cardTitle.classList.add("cardTitle");
        cardTitle.innerText = title;
    card.appendChild(cardPoster);
    card.appendChild(cardTitle);
    // Modal div control
    card.addEventListener("click", function (event) { document.getElementById(this.attributes["id"].value + "_modal").style.display = "block";});

    let modalDiv = document.createElement("div");
    modalDiv.classList.add("modalDiv");
    modalDiv.setAttribute("id", "card_"+id+"_modal");
        let modalContent = document.createElement("div");
        modalContent.classList.add("modalContent");
            let mil = document.createElement("div");
            mil.classList.add("movieInfo");
            mil.appendChild(cardPoster.cloneNode(true));
            let mir = document.createElement("div");
            mir.classList.add("movieInfo");
                let miTitle = document.createElement("h2");
                miTitle.innerText = title;
                let miDescr = document.createElement("p");
                miDescr.classList.add("description");
                miDescr.innerText = description;
                let play = document.createElement("a");
                play.classList.add("playButton");
                //TODO play button for movies
                play.setAttribute("id", "movie_file_name");
                play.setAttribute("href", "#");
                play.innerText = "play the movie";
            mir.appendChild(miTitle);
            mir.appendChild(miDescr);
            mir.appendChild(play);
        modalContent.appendChild(mil);
        modalContent.appendChild(mir);
    modalDiv.appendChild(modalContent);

    let output = document.createDocumentFragment();
    output.append(card);
    output.append(modalDiv);
    return output;
}

function setEventListeners(){
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
    refreshMovieCards("{\"data\": [{\"type\": \"file\",\"name\": \"Matrix\",\"file name\": \"Matrix (1999).mkv\"}]}");
    setEventListeners();
};*/