#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0xC0, 0x18, 0x08, 0x50, 0x10, 0xAE, 0x4E, 0x1C, 0x9F, 0x29, 0x95, 0x0A, 0xB0, 0x62, 0xCA, 0x27 }
PBL_APP_INFO(MY_UUID,
             "Coptic 2", "Your Company",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;
Layer display_layer;







char *cmonth[14] = {"NIL ", "Thout ", "Paope ", "Hathor ","Koiahk ","Tobe ","Meshir ","Paramhotep ","Parmoute ","Pashons ","Paone ","Epep ","Paope ","Nesi "};
char timeText[20]="Hello.";
char cdateText[20]=" ";
char gdateText[20]=" ";


/** Array used for fast number character lookup */
char *numbers1[10] = {"","1","2","3","4","5","6","7","8","9"};
char *numbers2[10] = {"0","1","2","3","4","5","6","7","8","9"};

void display_layer_update_callback(Layer *me, GContext* ctx) {
    (void)me;
    
    graphics_context_set_text_color(ctx, GColorWhite);
    graphics_context_set_fill_color(ctx, GColorWhite);
    
    graphics_text_draw(ctx,
                       timeText,
                       fonts_get_system_font(FONT_KEY_GOTHAM_42_BOLD),
                       GRect(0, 0, 144, 43),
                       GTextOverflowModeWordWrap,
                       GTextAlignmentCenter,
                       NULL);
    
  //  graphics_fill_rect(ctx, GRect(0, 47, 144, 3), 2, GCornersAll);
   
    graphics_draw_line(ctx,GPoint(1,49),GPoint(143,49));
    graphics_draw_line(ctx,GPoint(0,50),GPoint(144,50));
    graphics_draw_line(ctx,GPoint(1,51),GPoint(143,51));
    
    
    graphics_text_draw(ctx,
                       gdateText,
                       fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD),
                       GRect(2, 70, 142, 29),
                       GTextOverflowModeWordWrap,
                       GTextAlignmentLeft,
                       NULL);
    
    
    graphics_text_draw(ctx,
                       cdateText,
                       fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD),
                       GRect(2, 110, 142, 29),
                       GTextOverflowModeWordWrap,
                       GTextAlignmentRight,
                       NULL);
    
}




int julian(PblTm gdate)

{
    int I= gdate.tm_year+1900;
    int J= gdate.tm_mon + 1;
    int K= gdate.tm_mday;
    int JD= K-32075+1461*(I+4800+(J-14)/12)/4+367*(J-2-(J-14)/12*12)/12-3*((I+4900+(J-14)/12)/100)/4;
    return JD;
    
}


void update_dates( PblTm udate)

{
    
        // update gregorian date s strign
    
    string_format_time(gdateText, sizeof(gdateText),  "%a.  %b %e" ,&udate);
    
    
    // calculate julian day
    
    int JD=julian(udate);
    
    // calculate coptic month and day.
    
    if (udate.tm_hour>17) JD=JD+1;
    int r = ( (JD- 1824665) % 1461 ) ;
    int n = ( r % 365 ) + 365 * ( r/ 1460 ) ;
    //int year = 4 * ( (JD - 1824665)/ 1461 )+ ( r/ 365 )- ( r/ 1460 );
    int month = ( n/ 30 ) + 1 ;
    int day   = ( n% 30 ) + 1 ;
    
    
    // Convert then int day to the char cday
    
    char cday[10]= {'\0'};
    strcat(cday,numbers1[day / 10]);
    strcat(cday,numbers2[day % 10]);
    
    //combine coptic month + day
    
    strcpy(cdateText,cmonth[month]);
    strcat(cdateText,cday);

}












void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t)
{
    
    (void)t;
    (void)ctx;

    
    
    // update hour string
    
     string_format_time(timeText, sizeof(timeText), clock_is_24h_style() ? "%H:%M" : "%I:%M",t->tick_time);
    
  
    // if the hour changes update the datestring for possible changes
    
    if (t->units_changed == HOUR_UNIT) update_dates(*t->tick_time);
    
    
        
    // repaint the screen
    layer_mark_dirty(&display_layer);

}

void handle_init(AppContextRef ctx) {
    (void)ctx;
    
    window_init(&window, "Window Name");
    window_stack_push(&window, true /* Animated */);
    
    
    window_set_background_color(&window, GColorBlack);
    
    // Init the layer for the display
    
    layer_init(&display_layer, window.layer.frame);
    display_layer.update_proc = &display_layer_update_callback;
    layer_add_child(&window.layer, &display_layer);
    
    PblTm thetime;
    get_time(&thetime);
    
     update_dates(thetime);
     layer_mark_dirty(&display_layer);    
    
}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,

    
    .tick_info = {
        .tick_handler = &handle_minute_tick,
        .tick_units = MINUTE_UNIT
    }
  
    
};
    
    
    
    
    
    
  app_event_loop(params, &handlers);
}


