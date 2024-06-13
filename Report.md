Yapılan implementasyonlar:

list.c :
Senkronizasyon için semaphorelar, mutex ve condition eklendi.

view.c:
Droneları çizmek için kod bloğu eklendi.

controller.c:
drone_controller ve survivor_generator threadleri burada oluşturuldu.

model.c:
create_drone fonksiyonu dolduruldu. Gelen parametrelere göre drone
oluşturur.
help_survivor fonksiyonu dolduruldu. Drone survivor'a ulaştığında
survivorları listeden siler.
move_drone fonksiyonu dolduruldu. Droneların konumunu ayarlar.
drone_runner fonksiyonu dolduruldu. Yardıma ihtiyacı olan survivor u
bulur. Drone nu bu survivora yardım için gönderir.
drone_controller fonksiyonu dolduruldu. drone_runner threadlerini
oluşturur. Boşta olan drone u bulur ve drone_runner a gönderir. bu
işlemi 10(drone)*100 defa yapar.

drone_generator fonksiyonu eklendi. Droneları rastgele bir yerde oluşturur.
find_node_with_data fonksiyonu eklendi. Gelen survivorun listedeki node
adresini bulur.
find_survivor fonksiyonu eklendi. Yardım almamış survivorları bulur.
find_drone fonksiyonu eklendi. Boşta olan droneları bulur.
increase_helped fonksiyonu eklendi. Yardım edilenlerin sayısını arttırır.
check_and_remove_survivor_at_coord fonksiyonu eklendi. Drone nun
bulunduğu koordinatta survivor varsa ona yardım eder.

İstatistik:
Program saniyede x survivor üretmek ve her dronun saniyede 1 birimlik yol almasını
sağlamak üzere kodlanmıştır. x değeri olarak 1 alındığında droneların 1000 survivora
yardım sağlaması ortalama 50 dakika sürmektedir. x değeri arttıkça yardım sağlama süresi
de orantısal olarak artmaktadır. Drone hızları arttırıldığında bu süre azalır.
