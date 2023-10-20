
#include <stdio.h>
#include <tgbot/tgbot.h>
#include <vector>
#include <string>
#include<ctime>
#include <nlohmann/json.hpp>
#include <curl/curl.h>

static size_t Writer(char *buffer, size_t s, size_t nmemb, std::string *html){

  int k = 0;

  if(buffer != NULL){

    html->append(buffer, s*nmemb);
    k = s*nmemb;
    
  }
  return k;
  
}

std::string gr(std::string link){

  CURL *curl;
  std::string d;
  curl=curl_easy_init();
  curl_easy_setopt(curl,CURLOPT_URL, link.c_str());
  curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,Writer);
  curl_easy_setopt(curl,CURLOPT_WRITEDATA,&d);
  curl_easy_perform(curl);
  curl_easy_cleanup(curl);
  return d;
  
  
}

float gcur(char w){

  auto js = nlohmann::json::parse(gr("https://www.cbr-xml-daily.ru/daily_json.js"));

  if(w=='u'){

    return js["Valute"]["USD"]["Value"].get<float>();
    
  }
  else{

    return js["Valute"]["EUR"]["Value"].get<float>();
    
  }
  return -1;
}


std::string gtime(){

  time_t nw = time(NULL);
  tm* ptr = localtime(&nw);
  char bf[32];
  strftime(bf, 32, "%c", ptr);
  return bf;
  
}

std::vector<std::string>bcmds = {"start", "time", "currency"}; //bot commands

int main() {

  TgBot::Bot bot("insert_key");


  TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
  std::vector<TgBot::InlineKeyboardButton::Ptr> row0;
  TgBot::InlineKeyboardButton::Ptr usd(new TgBot::InlineKeyboardButton), eur(new TgBot::InlineKeyboardButton);
  usd->text = "USD";
  eur->text = "EUR";
  usd->callbackData = "USD";
  eur->callbackData = "EUR";
  row0.push_back(usd);
  row0.push_back(eur);
  keyboard->inlineKeyboard.push_back(row0);
  

  
    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Hi!");
    });

    bot.getEvents().onCommand("time", [&bot](TgBot::Message::Ptr message){
      bot.getApi().sendMessage(message->chat->id, "Current time:\n"+gtime());
      
    });

      bot.getEvents().onCommand("currency", [&bot, &keyboard](TgBot::Message::Ptr message){

	bot.getApi().sendMessage(message->chat->id, "What currency do you want?", false, 0, keyboard);
	
      
    });


    bot.getEvents().onCallbackQuery([&bot, &keyboard](TgBot::CallbackQuery::Ptr query){

      if(query->data == "USD"){

	bot.getApi().sendMessage(query->message->chat->id, std::to_string(gcur('u')));
	
	
      }else{

	bot.getApi().sendMessage(query->message->chat->id, std::to_string(gcur('e')));
	
      }
      
    });
    
 

    

    
    
    
    bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
        printf("User wrote '%s' but WTF I dont get it\n", message->text.c_str());
	
	for(const auto& c : bcmds){

	  if("/"+c==message->text){

	    return;
	    
	  }
	  
	}
	bot.getApi().sendPhoto(message->chat->id, TgBot::InputFile::fromFile("/home/izvr7/Desktop/CPP/Projects/TelegramBOt/7b3.jpg", "image/jpg"));
        bot.getApi().sendMessage(message->chat->id, "user wrote '" + message->text + "'" + " but WTF I dont get it");
    });

    
    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
    } catch (TgBot::TgException& e) {
        printf("error: %s\n", e.what());
    }
    return 0;
}
