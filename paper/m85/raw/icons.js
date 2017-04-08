
document.write("<DIV CLASS=\"top\" ID=\"top\" STYLE=\"position:absolute;top:1;display:none;\" ALIGN=RIGHT>");

document.write("</DIV>");
document.write("<DIV CLASS=\"footer\" ID=\"footer\" STYLE=\"position:absolute;top:1;display:none;\" ALIGN=RIGHT>");
document.write("<A HREF=/index.html><IMG SRC=/gif/page.gif BORDER=0></A>");
document.write("<A HREF=/born/born.html><IMG SRC=/gif/born.gif BORDER=0></A>");
document.write("<A HREF=/write/write.html><IMG SRC=/gif/write.gif BORDER=0></A>");
document.write("<A HREF=mailto:raw@neuron.et.ntust.edu.tw><IMG SRC=/gif/mail.gif BORDER=0></A>");
document.write("</DIV>");

var brand= null;

function toping()
{
  var alldivs= new Array();
  alldivs= document.all.tags("DIV");

  for (i=0; i<alldivs.length; i++)
  {
    if (alldivs(i).id == "top")
      brand= alldivs(i);
  }
  
  var IH= document.body.clientHeight;
  var IW= document.body.clientWidth;
  var PX= document.body.scrollLeft;
  var PY= document.body.scrollTop;
  var IMGW= 126;
  var IMGH= 49;
  var LSAFETY= 0;
  var TSAFETY= 0;
  if (IW>500)
  {
    brand.style.display= "none";
    brand.style.top=PY;
    brand.style.left=0;//IW+PX-(IMGW+LSAFETY)
    brand.style.display= "";
  }
  else brand.style.display= "none";
}

function footering()
{
  var alldivs= new Array();
  alldivs= document.all.tags("DIV");

  for (i=0; i<alldivs.length; i++)
  {
    if (alldivs(i).id == "footer")
      brand= alldivs(i);
  }
  
  var IH= document.body.clientHeight;
  var IW= document.body.clientWidth;
  var PX= document.body.scrollLeft;
  var PY= document.body.scrollTop;
  var IMGW= 126;
  var IMGH= 49;
  var LSAFETY= 0;
  var TSAFETY= 0;
  if (IW>500)
  {
    brand.style.display= "none";
    brand.style.top=IH+PY-(IMGH+TSAFETY);
    brand.style.left=0;//IW+PX-(IMGW+LSAFETY)
    brand.style.display= "";
  }
  else brand.style.display= "none";
}

toping();
footering();
window.setInterval("toping()",10);
window.setInterval("footering()",10);
document.write("<BR>");
document.write("<BR>");