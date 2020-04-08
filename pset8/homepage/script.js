let welcome = document.querySelector('#welcome');
welcome.addEventListener("mouseenter", function(event) {
    this.style.color = "red";
});
welcome.addEventListener("mouseout", function(event) {
    this.style.color = "black";
});