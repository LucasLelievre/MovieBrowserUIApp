// get request on given url
function requestAPI(url) {
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

function getMovieAPI(type, name, year, id, season, ep) {
    switch (type) {
        case "tvshow":
            return "https://api.themoviedb.org/3/search/tv?api_key=39556c17fbe9e58c430f6a811f19fb1c&query=" + encodeURIComponent(name);
        case "tvseason":
            return "https://api.themoviedb.org/3/tv/" + id + "/season/" + season + "?api_key=39556c17fbe9e58c430f6a811f19fb1c";
        case "tvep":
            return "https://api.themoviedb.org/3/tv/" + id + "/season/" + season + "/episode/" + ep + "?api_key=39556c17fbe9e58c430f6a811f19fb1c";
        default:
            // movie or collection
            return "https://api.themoviedb.org/3/search/" + type + "?api_key=39556c17fbe9e58c430f6a811f19fb1c&query="+encodeURIComponent(name)+"&year="+year;
    }
}

function getMovieDataFromURL(url, movie) {
    return requestAPI(url)
    .then(function (result) {
        // TODO what to do in case of no results
        if (result.results.length < 1) {
            if (movie.name == "Marvel Cinematic Universe") {
                movie["id"]="MCU";
                movie["original_title"]=movie.name;
                movie["poster_path"]="img/MCU.png";
                movie["overview"] = "Movies and TV series taking place in the Marvel Cinematic Universe";
                return movie
                //TODO does it work
            }
            console.log(url);
            return createErrorJSON(movie, "no results from API query");
        }

        let firstResult = result.results[0];
        // populates JSON object with online data
        movie["id"] = firstResult.id;
        movie["original_title"] = (movie.type == "movie") ? firstResult.original_title : firstResult.original_name;
        movie["poster_path"] = "https://image.tmdb.org/t/p/w500" + firstResult.poster_path;
        movie["overview"] = firstResult.overview;

        // If element is a TV season, look for more data
        if (movie.type == "tvseason") {
            return requestAPI(getMovieAPI(movie.type, "", movie.fileYear, movie.id, movie.season, ""))
            .then(function (result) {
                movie["id"] = result.id;
                movie["poster_path"] = "https://image.tmdb.org/t/p/w500" + result.poster_path
                movie["release_date"] = result.air_date;
                if (result.overview.length > 0) movie["overview"] = result.overview;
                return movie;
            }).catch(function(reason){
                return createErrorJSON(movie, "could not retreive TV season data");
            });
        }
        // If element is a TV episode, look for more data
        if (movie.type == "tvep") {
            return requestAPI(getMovieAPI(movie.type, "", movie.fileYear, movie.id, movie.season, movie.episode))
            .then(function (result) {
                movie["id"] = result.id;
                movie["original_title"] = result.name;
                movie["poster_path"] = "https://image.tmdb.org/t/p/w500" + result.still_path
                movie["release_date"] = result.air_date;
                if (result.overview.length > 0) movie["overview"] = result.overview;
                return movie;
            }).catch(function(reason){
                return createErrorJSON(movie, "could not retreive TV episode data");
            });
        }

        movie["release_date"] = firstResult.release_date;
        return movie;
    }).catch(function (reason) {
        if (movie.name == "") {
            return createErrorJSON(movie, "File name incorrectly parsed");
        }
        return createErrorJSON(movie, "an error occured while retreiving online data");
    });
}

// Update the cards from given JSON data
function refreshMovieCards(scannedData, cardListId, sort) {
    let cardList = document.getElementById(cardListId);
    let sortFunc = sort == "title" ? sortByOriginalTitle : sortByReleaseDate;

    console.log(scannedData);
    // empty the list
    cardList.innerHTML = "";
    let promises = [];

    // create a promise for each element in the JSON
    scannedData.content.forEach(element => {
        // create url request
        let url = "";
        if (element.type == "tvseason" || element.type == "tvep") {
            url = getMovieAPI("tvshow", element.name, element.fileYear, "", "", "");
        } else {
            url = getMovieAPI(element.type, element.name, element.fileYear, "", "", "");
        }
        promises.push(getMovieDataFromURL(url, element));
    });
    Promise.all(promises)
    .then(function (response) {
        // sort JSON objects
        response.sort(sortFunc);
        // Create and add the cards in html
        response.forEach(function (result) {
            cardList.appendChild(createCard(result));
        });
    }).catch(function (reason) {
        console.error("A problem occured while creating cards :");
        console.error(reason);
    });
}

function createErrorJSON(object, error){
    let obj = {
        type : "error",
        movie : object,
        reason : error
    }
    console.error(obj);
    return obj;
}

function createErrorCard(errorCard){
    let card = document.createElement("div");
    card.classList.add("card");
    let cardPoster = document.createElement("img");
    cardPoster.classList.add("cardPoster");
    cardPoster.setAttribute("alt", "error movie");
    cardPoster.setAttribute("src", "img/missing_poster.png");
    let cardTitle = document.createElement("div");
    cardTitle.classList.add("cardTitle");
    cardTitle.innerText = errorCard.movie.name;
    let cardInfo = document.createElement("div");
    cardInfo.classList.add("cardInfo");
    cardInfo.classList.add("item-float-left");
    cardInfo.innerText = errorCard.reason;
    card.appendChild(cardPoster);
    card.appendChild(cardTitle);
    card.appendChild(cardInfo);
    return card;
}

// Create a card visualisation of a JSON object
function createCard(cardData){
    if (cardData.type == "error") return createErrorCard(cardData);

    // Image on the card
    let cardPoster = document.createElement("img");
    cardPoster.classList.add("cardPoster");
    cardPoster.setAttribute("alt", "poster movie");
    cardPoster.setAttribute("src", cardData.poster_path);
    // Title of the card
    let cardTitle = document.createElement("div");
    cardTitle.classList.add("cardTitle");
    cardTitle.innerText = cardData.original_title;
    // Card itself
    let card = document.createElement("a");
    card.classList.add("card");
    card.classList.add("modalButton");
    card.setAttribute("id", "card_" + cardData.id);
    card.appendChild(cardPoster);
    card.appendChild(cardTitle);
    // Card type (Collection, tv, season, ep)
    let cardTVinfo = document.createElement("div");
    cardTVinfo.classList.add("cardInfo");
    cardTVinfo.classList.add("item-float-left");
    switch (cardData.type) {
        case "tvseason":
            cardTVinfo.innerText = "S" + cardData.season;
            break;
        case "tvep":
            cardTVinfo.innerText = "S" + cardData.season + "E" + cardData.episode;
            break;
        case "tvshow":
            cardTVinfo.innerText = "TV";
            break;
        case "collection":
            cardTVinfo.innerText = "Collection";
            break;
        default:
            break;
    }
    if (cardData.quality)
        cardTVinfo.innerText += '\t'+ cardData.quality;

    card.appendChild(cardTVinfo);
    // Release Date
    if (cardData.release_date != undefined) {
        // Release data if any
        let cardDate = document.createElement("div");
        cardDate.classList.add("cardInfo");
        cardDate.classList.add("item-float-right");
        cardDate.innerText = cardData.release_date;
        card.appendChild(cardDate);
    }
    // Modal box control
    card.addEventListener("click", function (event) { document.getElementById(this.attributes["id"].value + "_modal").style.display = "block"; });

    // Scan the inside of the directory and add the new cards
    if (cardData.type == "tvseason" || cardData.type == "collection" || cardData.type == "tvshow") {
        card.addEventListener("click", function(event) {
            let sublist = document.getElementById("sublist_"+cardData.id);
            if (sublist.innerHTML === "") {
                // TODO call c++ function to scann movies in filepath
                let sublistContent = scanDirectory(cardData.filename);
                refreshMovieCards(sublistContent, "sublist_"+cardData.id, "date");
            }
        });
    }

    // Image of the modal box
    let modalPoster = document.createElement("div");
    modalPoster.classList.add("modalPoster");
    modalPoster.appendChild(cardPoster.cloneNode(true));
    
    // Title of the modal box
    let modalTitle = document.createElement("h2");
    modalTitle.innerText = cardData.original_title;
    if (cardData.type == "tvseason") {
        let modalSeason = document.createElement("div");
        modalSeason.classList.add("modalSeason");
        modalSeason.innerText = "S" + cardData.season;
        modalTitle.appendChild(modalSeason);
    }
    if (cardData.quality){
        let modalQuality = document.createElement("div");
        modalQuality.classList.add("modalSeason");
        modalQuality.innerText = cardData.quality;
        modalTitle.appendChild(modalQuality);
    }

    // Close modal box Button
    let modalClose = document.createElement("a");
    modalClose.classList.add("item-float-right");
    modalClose.innerText = "(X)"
    // Desciption of the modal box
    let modalOverview = document.createElement("p");
    modalOverview.classList.add("overview");
    modalOverview.innerText = cardData.overview;
    // Container of the title, discriotion, playbutton, and sub list
    let modalInfo = document.createElement("div");
    modalInfo.classList.add("modalInfo");
    modalInfo.appendChild(modalClose);
    modalInfo.appendChild(modalTitle);
    
    modalInfo.appendChild(modalOverview);
    if (cardData.type == "movie" || cardData.type == "tvep") {
        // Play button for movies and TV episodes
        let play = document.createElement("a");
        play.classList.add("playButton");
        play.setAttribute("id", "movie_file_name");
        play.setAttribute("href", "#");
        play.innerText = "play the " + (cardData.type == "tvep" ? "episode" : "movie");
        // Callback C++ function that starts the movie in VLC
        play.addEventListener("click", function (event) { systemCommand("vlc", cardData.filename);});
        modalInfo.appendChild(play);
    } else {
        // Sub list of cards
        let subCollection = document.createElement("div");
        subCollection.classList.add("responsiveGrid");
        subCollection.setAttribute("id", "sublist_" + cardData.id);
        modalInfo.appendChild(subCollection);
        // refreshMovieCards(cardData, subCollection.getAttribute("id"), "date");
    }
    // Container of the modal box content
    let modalContent = document.createElement("div");
    modalContent.classList.add("modalContent");
    if (cardData.type == "tvep" || cardData.type == "movie") modalContent.classList.add("hasPlayButton");
    modalContent.appendChild(modalPoster);
    modalContent.appendChild(modalInfo);
    // Modal box itself
    let modalBox = document.createElement("div");
    modalBox.classList.add("modalBox");
    modalBox.setAttribute("id", "card_"+cardData.id+"_modal");
    modalBox.appendChild(modalContent);
    // return card and modal box
    let output = document.createDocumentFragment();
    output.append(card);
    output.append(modalBox);
    return output;
}

// Set the event listeners
function setEventListeners(){
    window.addEventListener("click", function(event){
        if(event.target.classList.contains("modalBox")) event.target.style.display = "none";
    });

    var anchors = document.getElementsByTagName("a");
    for (let anchor of anchors) {
        anchor.addEventListener("click", function(event){
            event.preventDefault()
        });
        if (anchor.classList.contains("weblink")) {
            anchor.addEventListener("click", function(event) {
                console.log(this.attributes["href"].value);
                // Call c++ function to launch webbrowser to url value
                systemCommand("firefox", this.attributes["href"].value);
            })
        }
    }

    document.getElementById("settings").addEventListener("click", function(event) {
        document.getElementById("settings_modal").style.display = "block";
    });
    document.getElementById("settingsSave").addEventListener("click", function(event) {
        document.getElementById("settings_modal").style.display = "none";
        // saveSettings("pouetpouetBonjour");
    });
}

/*window.onload = function(){
    let input = "{\"content\":[\
        {\"type\": \"movie\",\"name\": \"xmen\",\"file name\": \"the matrix.mkv\"},\
        {\"type\": \"collection\",\"name\":\"james bond\",\"content\":[\
            {\"type\": \"movie\",\"name\": \"skyfall\",\"file name\": \"skyfall.mkv\"}]},\
        {\"type\": \"tvshow\",\"name\": \"witcher\",\"file name\": \"witcher (TV)\",\"content\":[\
            {\"type\": \"tvseason\",\"name\": \"the witcher\", \"season\":\"1\",\"file name\": \"the witcher (S01)\",\"content\":[\
                {\"type\":\"tvep\",\"name\":\"the witcher\", \"season\":\"1\", \"episode\":\"1\",\"file name\": \"the witcher (S01E01)\"}]}]},\
        {\"type\": \"tvseason\",\"name\": \"loki\", \"season\":\"1\",\"file name\": \"loki (S01)\",\"content\":[\
            {\"type\":\"tvep\",\"name\":\"loki\", \"season\":\"1\", \"episode\":\"1\",\"file name\": \"loki (S01E01)\"}]}\
    ]}";
    // console.log(input);
    refreshMovieCards(JSON.parse(input), "movieList", "title");
    setEventListeners();
};*/