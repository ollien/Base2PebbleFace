#include <pebble.h>
#include <stdlib.h>
Window *my_window;
TextLayer *text_layer;
int rowNum=1;
time_t now;
Layer * display_layer;
char *dayBuffer;
struct tm * timePoint;
Layer * window_layer;
char numChar[3];
char *bases[]={"1","2","4","8","16","32"};
int fourth = 0;
TextLayer * numLayers[19];
int textLayerIndex=0;
int progressCount=0;
int runCount;

char* itoa(int val, int base){
	
	static char buf[32] = {0};
	
	int i = 30;
	
	for(; val && i ; --i, val /= base)
	
		buf[i] = "0123456789abcdef"[val % base];
	
	return &buf[i+1];
	
}
 char * toBinary(int num, int slots){
	char * buffer =malloc (slots+1);
	int n,c,k;
	n=num;
	int i=0;
	int count=32;
//	int oneHit=0;
    for (c = 31; c >= 0; c--)
     {
       k = n >> c;

       if (k & 1){
		   if (count<=slots){
		   buffer[i]='1';
		   i++;
	   		}
	   }
       else if (count<=slots){
			buffer[i]='0';
		   i++;
	   }
	   count--;
     }
	 return buffer;
}
int stoi(char *str)
{
	int res = 0;
    for (int i = 0; str[i] != '\0'; ++i)
        res = res*10 + str[i] - '0';
	free(str);
    return res;
}

void revString(char *str){
 int i;
 int length;
 int last_pos;
 length = strlen(str);
 last_pos = length-1;

 for(i = 0; i < length / 2; i++)
 {
   char tmp = str[i];
   str[i] = str[last_pos - i];
   str[last_pos - i] = tmp;
 }
}
int powIt2(int n){ 
	int ans = 2; 
	if (n==0){
		return 1;
	}
	else if (n==1){
		return 2;
	}
	else{
		for (int i=1;i<n;i++) 
			ans *= 2; 
	}
		return ans; 
} 
void draw_dot_with_text(GContext *ctx,Layer * layer, GPoint center, bool filled, int num){	
	graphics_context_set_fill_color(ctx,GColorWhite);
	graphics_context_set_stroke_color(ctx,GColorWhite);
	
	if (filled){
		graphics_fill_circle(ctx, center, 10);	
	}
	else{
		graphics_draw_circle(ctx, center, 10);	
	}
	
	numLayers[textLayerIndex] = text_layer_create(GRect(center.x-4,center.y-25,50,90));
	text_layer_set_text(numLayers[textLayerIndex], bases[num]);
	text_layer_set_background_color(numLayers[textLayerIndex], GColorClear);
	text_layer_set_text_color(numLayers[textLayerIndex], GColorWhite);
	layer_add_child(layer,text_layer_get_layer(numLayers[textLayerIndex]));
	textLayerIndex++;

}

//if timeDot is true, do time, else, do date
void draw_row(GContext *ctx,Layer *layer,unsigned int count,bool timeDot){
	int x=15;
	time(&now);
	timePoint = localtime(&now);
	char timeBuffer[3];
	char binaryBuffer[7];
	if (timeDot){
		if (count==4 || count==5){
			strftime(timeBuffer,3,"%H",timePoint);	
			
		}
		else if (count==6){
			strftime(timeBuffer,3,"%M",timePoint);
		}
		int num; 
		if (count==4){
			num=stoi(timeBuffer)%12;
			if (num==0){
				num=12;
			}
		}
		else{
			num=stoi(timeBuffer);
		}
		char * itoaResult=toBinary(num,count);
		strcpy(binaryBuffer,itoaResult);
		free(itoaResult);
	}
	else{
		char dayBuffer[3];
		if (count==4){
			strftime(dayBuffer,3,"%D",timePoint);	
		}
		if (count==5){
			strftime(dayBuffer,3,"%d",timePoint);	
		}
		int num = stoi(dayBuffer);
		char * itoaResult=toBinary(num,count);
		strcpy(binaryBuffer,itoaResult);
		free(itoaResult);
	}
	int q=count-1;
	for (unsigned int i=0; i<count; i++){
		char check;
		if (i<strlen(binaryBuffer)){
			check = binaryBuffer[i];
		}
		else{
		 	check='0';
		}
		if (check=='1')
			draw_dot_with_text(ctx,layer,GPoint(x,30*rowNum),true,q);
		else
			draw_dot_with_text(ctx,layer,GPoint(x,30*rowNum),false,q);
		q--;
		x+=23;
	}
	rowNum++;
	if (rowNum==3){
		rowNum++;
	}
}
void seconds_bar_callback(Layer * layer, GContext *ctx,int fourth){
	graphics_context_set_fill_color(ctx,GColorWhite);
	graphics_context_set_stroke_color(ctx,GColorWhite);
	graphics_fill_rect(ctx, GRect(0,75,35*fourth,5), 0, GCornerNone);
}
void add_day_of_week(Layer * layer, GContext * ctx){
	text_layer = text_layer_create(GRect(0,69,144,25));
	text_layer_set_text_alignment(text_layer,GTextAlignmentCenter);
	text_layer_set_text_color(text_layer, GColorWhite);
	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21 ));
	text_layer_set_background_color(text_layer, GColorClear);
	time(&now);
	timePoint = localtime(&now);
	dayBuffer="Failssday";
	switch(timePoint->tm_wday){
		case 0:
			dayBuffer="Sunday";
			break;
		case 1:
			dayBuffer="Monday";
			break;
		case 2:
			dayBuffer="Tuesday";
			break;
		case 3:
			dayBuffer="Wednesday";
			break;
		case 4:
			dayBuffer="Thursday";
			break;
		case 5:
			dayBuffer="Friday";
			break;
		case 6:
			dayBuffer="Saturday";
			break;
	}
	
			
	text_layer_set_text(text_layer,dayBuffer);
	layer_add_child(layer, text_layer_get_layer(text_layer));
}

void draw_layer_update_proc(Layer * layer, GContext *ctx){
	if (clock_is_24h_style()){
		draw_row(ctx,layer,5,true);
	}
	else{
		draw_row(ctx,layer,4,true);
	}
	draw_row(ctx,layer,6,true);
	//Date row
	draw_row(ctx,layer,4,false);
	draw_row(ctx,layer,5,false);
	add_day_of_week(layer,ctx);
}

void handle_time_change(struct tm *tick_time, TimeUnits units_changed){ 
	runCount++;
	rowNum=1;
	for (int i=0; i<19; i++){
		text_layer_destroy(numLayers[i]);
	}
	textLayerIndex=0;
	
	
	//layer_remove_child_layers(display_layer);
	text_layer_destroy(text_layer);
	layer_mark_dirty(display_layer);
	free(tick_time);
}

void bluetooth_connection_callback(bool connected){
	if (!connected){
		vibes_double_pulse();
	}
}

void handle_init(void) {
	  my_window = window_create();
	window_stack_push(my_window, true);
	window_set_background_color(my_window, GColorBlack);
//	  text_layer = text_layer_create(GRect(0, 0, 144, 20));
	window_layer = window_get_root_layer(my_window);
	display_layer = layer_create(layer_get_bounds(window_layer));
	layer_set_update_proc(display_layer, draw_layer_update_proc);
	layer_add_child(window_layer, display_layer);
	tick_timer_service_subscribe(SECOND_UNIT,handle_time_change);
	 bluetooth_connection_service_subscribe(bluetooth_connection_callback);
}

void handle_deinit(void) {
	  text_layer_destroy(text_layer);
		layer_remove_child_layers(display_layer);
	layer_mark_dirty(display_layer);
	  window_destroy(my_window);
	
	tick_timer_service_unsubscribe();
}

int main(void) {
	  handle_init();
	  app_event_loop();
	  handle_deinit();
}