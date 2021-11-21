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

function sortByReleaseDate(a,b) {
    return new Date(a.release_date).getTime() - new Date(b.release_date).getTime();
}
function sortByOriginalTitle(a, b) {
    return a.original_title > b.original_title;
}

function refreshMovieCards(scannedData) {
    movieList = document.getElementById("movieList");
    movieList.innerHTML = "";
    let promises = [];

    JSON.parse(scannedData).data.forEach(element => {
        let url = "";
        switch (element.type) {
            case "directory":
                url = "https://api.themoviedb.org/3/search/collection?api_key=39556c17fbe9e58c430f6a811f19fb1c&query="+encodeURIComponent(element.name);
                console.log(url);
                break;
            case "file":
                url = "https://api.themoviedb.org/3/search/movie?api_key=39556c17fbe9e58c430f6a811f19fb1c&query="+encodeURIComponent(element.name);
            default:
                url = "https://api.themoviedb.org/3/search/multi?api_key=39556c17fbe9e58c430f6a811f19fb1c&query="+encodeURIComponent(element.name);
                break;
        }
        promises.push(
            getDataFromURL(url)
            .then(function (result) {
                let firstResult = result.results[0];
                //console.log(firstResult);
                element["id"] = firstResult.id;
                element["original_title"] = (element.type == "file") ? firstResult.original_title : firstResult.original_name;

                element["poster_path"] = firstResult.poster_path;
                element["overview"] = firstResult.overview;
                element["release_date"] = firstResult.release_date;
                return element;
                //movieList.appendChild(createCard(movie.id, movie.original_title, "https://image.tmdb.org/t/p/w500" + movie.poster_path, movie.overview));
            }).catch(function (reason) {
                console.log(url);
                console.error(reason);
                //movieList.appendChild(createCard(0, "Error API", "img/missing_poster.png", reason, "error api"));
            })
        );
    });
    Promise.all(promises)
    .then(function (response) {
        response.sort(sortByOriginalTitle);
        response.forEach(function (result) {
            movieList.appendChild(createCard(result, "https://image.tmdb.org/t/p/w500"));
        });
    }).catch(function (reason) {
        console.error(reason);
    });
}

function createCard(movie, posterURL){
    let card = document.createElement("a");
    card.classList.add("card");
    card.classList.add("modalButton");
    card.setAttribute("id", "card_"+movie.id);
        let cardPoster = document.createElement("img");
        cardPoster.classList.add("cardPoster");
        cardPoster.setAttribute("alt", "movie cardPoster");
        cardPoster.setAttribute("src", posterURL+movie.poster_path);
        let cardTitle = document.createElement("div");
        cardTitle.classList.add("cardTitle");
        cardTitle.innerText = movie.original_title;
    card.appendChild(cardPoster);
    card.appendChild(cardTitle);
    if (movie.release_date != undefined) {
        console.log(movie.release_date);
        let cardDate = document.createElement("div");
        cardDate.classList.add("cardDate");
        cardDate.innerText = "(" + movie.release_date + ")";
        card.appendChild(cardDate);
    }
    // Modal div control
    card.addEventListener("click", function (event) { document.getElementById(this.attributes["id"].value + "_modal").style.display = "block";});

    let modalDiv = document.createElement("div");
    modalDiv.classList.add("modalDiv");
    modalDiv.setAttribute("id", "card_"+movie.id+"_modal");
        let modalContent = document.createElement("div");
        modalContent.classList.add("modalContent");
            let mil = document.createElement("div");
            mil.classList.add("movieInfo");
            mil.appendChild(cardPoster.cloneNode(true));
            let mir = document.createElement("div");
            mir.classList.add("movieInfo");
                let miTitle = document.createElement("h2");
                miTitle.innerText = movie.original_title;
                let miDescr = document.createElement("p");
                miDescr.classList.add("description");
                miDescr.innerText = movie.overview;
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
    refreshMovieCards("{\"data\":[{\"type\": \"file\",\"name\": \"the matrix\",\"file name\": \"the matrix.mkv\"},{\"type\": \"directory\",\"name\":\"james bond\",\"content\":[]},{\"type\": \"file\",\"name\": \"captain america first avenger\",\"file name\": \"captain america first avenger.mkv\"},{\"type\": \"directory\",\"name\":\"guillermo del toro\",\"content\":[]}]}");
    setEventListeners();
};*/