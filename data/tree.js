function setProgram(nr, brightness=255, speed=100, color1="ffffff", color2="ffffff") {
	var xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
		   // Typical action to be performed when the document is ready:
		   document.getElementById("demo").innerHTML = xhttp.responseText;
		}
	};
	xhttp.open("GET", `/prg?nr=${nr}&brightness=${brightness}&speed=${speed}&color1=${color1}&color2=${color2}`, true);
	xhttp.send();
}
