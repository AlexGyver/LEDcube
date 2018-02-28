![PROJECT_PHOTO](https://github.com/AlexGyver/LEDcube/blob/master/proj_img.jpg)
# Светодиодный куб 8x8x8 на Arduino
* [Описание проекта](#chapter-0)
* [Папки проекта](#chapter-1)
* [Схемы подключения](#chapter-2)
* [Материалы и компоненты](#chapter-3)
* [Как скачать и прошить](#chapter-4)
* [FAQ](#chapter-5)
* [Полезная информация](#chapter-6)
[![AlexGyver YouTube](http://alexgyver.ru/git_banner.jpg)](https://www.youtube.com/channel/UCgtAOyEQdAyjvm9ATCi_Aig?sub_confirmation=1)

<a id="chapter-0"></a>
## Описание проекта
- Светодиодный куб 8x8x8
- 10 режимов
- Настройка скорости анимации
- Динамическая индикация
- Подробности в видео: 

<a id="chapter-1"></a>
## Папки
**ВНИМАНИЕ! Если это твой первый опыт работы с Arduino, читай [инструкцию](#chapter-4)**
- **libraries** - библиотеки проекта. Заменить имеющиеся версии
- **CUBE_Gyver** - прошивка для Arduino, файл в папке открыть в Arduino IDE ([инструкция](#chapter-4))
- **schemes** - схемы и печатки

<a id="chapter-2"></a>
## Схемы
+ Весь проект на EasyEDA [ссылка](https://easyeda.com/beragumbo/LED_Cube-8937fd8460054d639d93736e35345182)
+ Печатная плата из видео [ссылка](https://easyeda.com/editor#id=853291a217984a219d6b2c69a1e57860)
+ **Gerber файлы** в папке schemes в архиве проекта!
Как вывести негатив для самодельной печатной платы я показывал в своих уроках по созданию печатных плат:
- https://youtu.be/Rbf7AZkfNwQ
- https://youtu.be/NJTeIALlztI
- https://youtu.be/2sISKRJSLBo
![SCHEME](https://github.com/AlexGyver/LEDcube/blob/master/schemes/scheme.jpg)

<a id="chapter-3"></a>
## Материалы и компоненты
### Ссылки оставлены на магазины, с которых я закупаюсь уже не один год
* Arduino NANO с ногами http://ali.pub/2avsvp http://ali.pub/2avsw3
* Arduino NANO без ног http://ali.pub/2avsyf http://ali.pub/2avsz7
* Светодиоды с длинными ногами 100 штук
	+ Синие http://ali.pub/2avtgw
	+ Розовые http://ali.pub/2avtha
	+ Красные http://ali.pub/2avti2
	+ Зелёные http://ali.pub/2avtj1
	+ Жёлтые http://ali.pub/2avtk7
* Светодиоды по 10 штук
	+ Синие http://ali.pub/2avtnp
	+ Розовые http://ali.pub/2avtrz
	+ Красные http://ali.pub/2avtw2
	+ Жёлтые http://ali.pub/2avu14
	+ Зелёные http://ali.pub/2avu1i
* Сдвиговые регистры 74hc595n http://ali.pub/2avx3k
* Транзисторы bd241c http://ali.pub/2avx20
* Резисторы http://ali.pub/2avx54
* Транзисторы TIP41C (на всякий случай) http://ali.pub/2avx63
* Макетка 8 на 12 см http://ali.pub/2avxad
* Гребёнка http://ali.pub/2avxen
* Кнопки и стойки ищите в любых магазинах для радиолюбителей, так как у китайцев можно купить только мешок 100 штук!

## Вам скорее всего пригодится
* [Всё для пайки (паяльники и примочки)](http://alexgyver.ru/all-for-soldering/)
* [Недорогие инструменты](http://alexgyver.ru/my_instruments/)
* [Все существующие модули и сенсоры Arduino](http://alexgyver.ru/arduino_shop/)
* [Электронные компоненты](http://alexgyver.ru/electronics/)
* [Аккумуляторы и зарядные модули](http://alexgyver.ru/18650/)

<a id="chapter-4"></a>
## Как скачать и прошить
* [Первые шаги с Arduino](http://alexgyver.ru/arduino-first/) - ультра подробная статья по началу работы с Ардуино, ознакомиться первым делом!
* Скачать архив с проектом
> На главной странице проекта (где ты читаешь этот текст) вверху справа зелёная кнопка **Clone or download**, вот её жми, там будет **Download ZIP**
* Установить библиотеки в  
`C:\Program Files (x86)\Arduino\libraries\` (Windows x64)  
`C:\Program Files\Arduino\libraries\` (Windows x86)
* Подключить Ардуино к компьютеру
* Запустить файл прошивки (который имеет расширение .ino)
* Настроить IDE (COM порт, модель Arduino, как в статье выше)
* Настроить что нужно по проекту
* Нажать загрузить
* Пользоваться  

## Настройки в коде
    #define INVERT_Y 1    // инвертировать по вертикали (если дождь идёт вверх)
    #define INVERT_X 0    // инвертировать по горизонтали (если текст не читается)

    // текст для режима текста
    String disp_text = "AlexGyver xyz";

<a id="chapter-5"></a>
## FAQ
### Основные вопросы
В: Как скачать с этого грёбаного сайта?  
О: На главной странице проекта (где ты читаешь этот текст) вверху справа зелёная кнопка **Clone or download**, вот её жми, там будет **Download ZIP**

В: Скачался какой то файл .zip, куда его теперь?  
О: Это архив. Можно открыть стандартными средствами Windows, но думаю у всех на компьютере установлен WinRAR, архив нужно правой кнопкой и извлечь.

В: Я совсем новичок! Что мне делать с Ардуиной, где взять все программы?  
О: Читай и смотри видос http://alexgyver.ru/arduino-first/

В: Компьютер никак не реагирует на подключение Ардуины!  
О: Возможно у тебя зарядный USB кабель, а нужен именно data-кабель, по которому можно данные передавать

В: Ошибка! Скетч не компилируется!  
О: Путь к скетчу не должен содержать кириллицу. Положи его в корень диска.

В: Сколько стоит?  
О: Ничего не продаю.

### Вопросы по этому проекту

<a id="chapter-6"></a>
## Полезная информация
* [Мой сайт](http://alexgyver.ru/)
* [Основной YouTube канал](https://www.youtube.com/channel/UCgtAOyEQdAyjvm9ATCi_Aig?sub_confirmation=1)
* [YouTube канал про Arduino](https://www.youtube.com/channel/UC4axiS76D784-ofoTdo5zOA?sub_confirmation=1)
* [Мои видеоуроки по пайке](https://www.youtube.com/playlist?list=PLOT_HeyBraBuMIwfSYu7kCKXxQGsUKcqR)
* [Мои видеоуроки по Arduino](http://alexgyver.ru/arduino_lessons/)