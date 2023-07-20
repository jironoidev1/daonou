const dino = document.getElementById("dino");
const cactus = document.getElementById("cactus");

function GOver() {
  alert('Game Over');
}

function jump() {
  if (dino.classList != "jump") {
    dino.classList.add("jump");

    setTimeout(function () {
      dino.classList.remove("jump");
    }, 330);
  }
}

let isAlive = setInterval(function () {
  // get current dino Y position
  let dinoTop = parseInt(window.getComputedStyle(dino).getPropertyValue("top"));

  // get current cactus X position
  let cactusLeft = parseInt(
    window.getComputedStyle(cactus).getPropertyValue("left")
  );

  // detect collision
  if (cactusLeft < 60 && cactusLeft > 20 && dinoTop >= 295) {
    // collision
    GOver();
  }
}, 10);

document.addEventListener("keydown", function (event) {
  jump();
});

document.addEventListener("click", function (event) {
  jump();
});
