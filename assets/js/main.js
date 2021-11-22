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

// Get a TV show's ID online
function getTVshowId(name) {
    return getDataFromURL("https://api.themoviedb.org/3/search/tv?api_key=39556c17fbe9e58c430f6a811f19fb1c&query="+encodeURIComponent(name));
}

// sort JSON data by date
function sortByReleaseDate(a,b) {
    return new Date(a.release_date).getTime() - new Date(b.release_date).getTime();
}
// sort JSON data by title
function sortByOriginalTitle(a, b) {
    return a.original_title > b.original_title;
}

// Update the cards from given JSON data
function refreshMovieCards(scannedData) {
    movieList = document.getElementById("movieList");
    // empty the list
    movieList.innerHTML = "";
    let promises = [];

    // create a promise for each element in the JSON
    JSON.parse(scannedData).data.forEach(element => {
        // create url request
        let url = "";
        switch (element.type) {
            case "collection":
                url = "https://api.themoviedb.org/3/search/collection?api_key=39556c17fbe9e58c430f6a811f19fb1c&query="+encodeURIComponent(element.name);
                break;
            case "movie":
                url = "https://api.themoviedb.org/3/search/movie?api_key=39556c17fbe9e58c430f6a811f19fb1c&query="+encodeURIComponent(element.name);
                break;
            case "tvshow":
                url = "https://api.themoviedb.org/3/search/tv?api_key=39556c17fbe9e58c430f6a811f19fb1c&query="+encodeURIComponent(element.name);
                break;
            case "tvseason":
                url = "https://api.themoviedb.org/3/search/tv?api_key=39556c17fbe9e58c430f6a811f19fb1c&query="+encodeURIComponent(element.name);
                break;
            default:
                url = "https://api.themoviedb.org/3/search/multi?api_key=39556c17fbe9e58c430f6a811f19fb1c&query="+encodeURIComponent(element.name);
                break;
        }
        promises.push(
            getDataFromURL(url)
            .then(function (result) {
                // TODO what to do in case of no results
                let firstResult = result.results[0];
                // populates JSON object with online data
                element["id"] = firstResult.id;
                element["original_title"] = (element.type == "movie") ? firstResult.original_title : firstResult.original_name;
                element["poster_path"] = firstResult.poster_path;
                element["overview"] = firstResult.overview;

                // If element is a TV season, look for more data
                if (element.type == "tvseason") {
                    return getDataFromURL("https://api.themoviedb.org/3/tv/"+element.id+"/season/"+element.season+"?api_key=39556c17fbe9e58c430f6a811f19fb1c")
                    .then(function (result) {
                        element["id"] = result.id;
                        element["release_date"] = result.air_date;
                        element["episodes"] = result.episodes;
                        if (result.overview.length > 0) element["overview"] = result.overview;
                        return element;
                    }).catch(function(reason){
                        // TODO create error card
                        console.error("Could not retrieve TV season data :");
                        console.error(reason);
                    });
                } else {
                    element["release_date"] = firstResult.release_date;
                }
                return element;
            }).catch(function (reason) {
                // TODO create error card
                console.log(url);
                console.error("A problem occured while retreive online data :");
                console.error(reason);
                //movieList.appendChild(createCard(0, "Error API", "img/missing_poster.png", reason, "error api"));
            })
        );
    });
    Promise.all(promises)
    .then(function (response) {
        console.log(response);
        // sort JSON objects
        response.sort(sortByOriginalTitle);
        // Create and add the cards in html
        response.forEach(function (result) {
            movieList.appendChild(createCard(result, "https://image.tmdb.org/t/p/w500"));
        });
    }).catch(function (reason) {
        console.error("A problem occured while create cards :");
        console.error(reason);
    });
}

// Create a card visualisation of a JSON object
function createCard(cardData, posterURL){
    let card = document.createElement("a");
    card.classList.add("card");
    card.classList.add("modalButton");
    card.setAttribute("id", "card_"+cardData.id);
        let cardPoster = document.createElement("img");
        cardPoster.classList.add("cardPoster");
        cardPoster.setAttribute("alt", "movie cardPoster");
        cardPoster.setAttribute("src", posterURL+cardData.poster_path);
        let cardTitle = document.createElement("div");
        cardTitle.classList.add("cardTitle");
        cardTitle.innerText = cardData.original_title;
    card.appendChild(cardPoster);
    card.appendChild(cardTitle);
    if (cardData.type == "tvseason") {
        let cardSeason = document.createElement("span");
        cardSeason.classList.add("cardTVep");
        cardSeason.innerText = "Season "+cardData.season;
        cardTitle.appendChild(cardSeason);
    }
    if (cardData.release_date != undefined) {
        let cardDate = document.createElement("div");
        cardDate.classList.add("cardDate");
        cardDate.innerText = "(" + cardData.release_date + ")";
        card.appendChild(cardDate);
    }
    // Modal div control
    card.addEventListener("click", function (event) { document.getElementById(this.attributes["id"].value + "_modal").style.display = "block";});

    let modalDiv = document.createElement("div");
    modalDiv.classList.add("modalDiv");
    modalDiv.setAttribute("id", "card_"+cardData.id+"_modal");
        let modalContent = document.createElement("div");
        modalContent.classList.add("modalContent");
            let mil = document.createElement("div");
            mil.classList.add("movieInfo");
            mil.appendChild(cardPoster.cloneNode(true));
            let mir = document.createElement("div");
            mir.classList.add("movieInfo");
                let miTitle = document.createElement("h2");
                miTitle.innerText = cardData.original_title;
                let miDescr = document.createElement("p");
                miDescr.classList.add("description");
                miDescr.innerText = cardData.overview;
            mir.appendChild(miTitle);
            mir.appendChild(miDescr);
            if (cardData.type == "movie"){
                let play = document.createElement("a");
                play.classList.add("playButton");
                //TODO play button for movies
                play.setAttribute("id", "movie_file_name");
                play.setAttribute("href", "#");
                play.innerText = "play the movie";
                mir.appendChild(play);
            }
        modalContent.appendChild(mil);
        modalContent.appendChild(mir);
    modalDiv.appendChild(modalContent);

    let output = document.createDocumentFragment();
    output.append(card);
    output.append(modalDiv);
    return output;
}

// Set the event listeners
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

window.onload = function(){
    refreshMovieCards(
        "{\"data\":[{\"type\": \"movie\",\"name\": \"the matrix\",\"file name\": \"the matrix.mkv\"},\
        {\"type\": \"collection\",\"name\":\"james bond\",\"content\":[]},\
        {\"type\": \"tvshow\",\"name\": \"witcher\",\"file name\": \"witcher (TV)\"},\
        {\"type\": \"tvseason\",\"name\": \"loki\", \"season\":\"1\",\"file name\": \"loki (S01)\"}]}");
    setEventListeners();
};