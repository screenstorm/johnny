$(function() {
    $( "#message_div" ).hide();
  
    $('#cp3').colorpicker({
        //color: '#AA3399',
        //format: 'rgb'
    });
    
    var QueryString = function () {
      var query_string = {};
      var query = window.location.search.substring(1);
      var vars = query.split("&");
      for (var i=0;i<vars.length;i++) {
        var pair = vars[i].split("=");
        if (typeof query_string[pair[0]] === "undefined") {
          query_string[pair[0]] = decodeURIComponent(pair[1]);
        } else if (typeof query_string[pair[0]] === "string") {
          var arr = [ query_string[pair[0]],decodeURIComponent(pair[1]) ];
          query_string[pair[0]] = arr;
        } else {
          query_string[pair[0]].push(decodeURIComponent(pair[1]));
        }
      } 
      return query_string;
    }();
    
    if(QueryString.access_token == undefined){
      $( "#message_div" ).show();
      $( "#message" ).empty().append( "No access_token GET parameter found!" );
    }
    
    // Attach a submit handler to the form
    $( "#customForm" ).submit(function( event ) {
      $( "#result" ).empty().append( '<pre>Submitting...</pre>' );
      
      // Stop form from submitting normally
      event.preventDefault();
     
      var $form = $( this );
      
      //rgb formatting
      var rgb = $form.find( "input[name='ledColor']" ).val();
      rgb = rgb.replace('rgb(', '');
      rgb = rgb.replace(')', '');
     
      var lcdLine1 = $form.find( "input[name='line1']" ).val(),
        lcdLine2 = $form.find( "input[name='line2']" ).val(),
        ledMode = $form.find( "select[name='ledMode']" ).val(),
        str = 'LCDLINE1='+lcdLine1+',LCDLINE2='+lcdLine2+',LEDMODE='+ledMode+',COLOR1='+rgb,
        url = $form.attr( "action" )+'?access_token='+QueryString.access_token;
     
      // Send the data using post
      var posting = $.post( url, { args: str } );
     
      posting.done(function( data ) {
        $( "#result" ).empty().append( "<pre>"+JSON.stringify(data, undefined, 2)+'</pre>' );
      });
      posting.fail(function( data ) {
        $( "#result" ).empty().append( "<pre>"+JSON.stringify(data.responseJSON, undefined, 2)+'</pre>' );
      });
    });
    
    
    
    console.log(QueryString.access_token);
});