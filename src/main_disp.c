#include "../sig_exchange_lib/sig_lib.h"
#include "boot_init.h"
#include "../tbv_lib/tbv.h"
#include "stdlib.h"

//Список необходимых ПМ
struct PM_INFO pm_pack[] = {
		{
				"read_reg",
				"/bootrofs_SA11_0/read_reg.gz",
				50
		},

		{
				"write_reg",
				"/bootrofs_SA14_0/write_reg.gz",
				50
		}

};

//Индексы модулей
#define READ_REG 0
#define WRITE_REG 1

//Инициализация всех ПМ
static errcode_t create_pm(int8_t);

//Описание всех сигналов
void signal_description(SIGSELECT);

int main(void){
	print_debug("Запуск диспетчера\r\n");
	delay(5000);

	//Установка и запуск
	create_pm(AUTO_START);

	//Инициализация ПМ read_reg
	do{
		send_sig_module_em(pm_pack[READ_REG].name, PM_STATUS_INI);
	}while(rec_w_sig_module(pm_pack[READ_REG].name, signal_description, pm_pack[READ_REG].timeout_kvit) != PM_STATUS_INI);
	//!Инициализация ПМ read_reg

	//Инициализация ПМ write_reg
	do{
		send_sig_module_em(pm_pack[WRITE_REG].name, PM_STATUS_INI);
	}while(rec_w_sig_module(pm_pack[WRITE_REG].name, signal_description, pm_pack[WRITE_REG].timeout_kvit) != PM_STATUS_INI);
	//!Инициализация ПМ write_reg

	//Работа ПМ

	//Запись в ЗП
	send_sig_module(pm_pack[WRITE_REG].name, PM_STATUS_START);
	while(rec_w_sig_module(pm_pack[WRITE_REG].name, signal_description, pm_pack[WRITE_REG].timeout_kvit) != PM_STATUS_END);

	//Чтение ЗП
	send_sig_module(pm_pack[READ_REG].name, PM_STATUS_START);
	while(rec_w_sig_module(pm_pack[READ_REG].name, signal_description, pm_pack[READ_REG].timeout_kvit) != PM_STATUS_END);

	//!Работа ПМ

	print_debug("Завершение работы диспетчера\r\n");
	stop(current_process());
	return 0;
}

/* Функция описывает поведение диспетчера при получении
 * сигнала от модуля.
 * */
void signal_description(SIGSELECT sig_no){
	switch(sig_no){
        case PM_STATUS_INI:{
            print_debug("Модуль был инициализирован\r\n");
            break;
        }
        case PM_STATUS_READY:{
            print_debug("Модуль готов к работе\r\n");
            break;
        }
        case PM_STATUS_START:{
            print_debug("Произошел старт модуля\r\n");
            break;
        }
		case PM_STATUS_SUCCESS:{
			print_debug("Модуль доступен\r\n");
			break;
		}
		case PM_STATUS_END:{
			print_debug("Модуль завершил работу\r\n");
			break;
		}
		default:{
			print_debug("Сигнал не был проиндексирован\r\n");
			break;
		}
    }
}

/*
 * Функция использует массив из struct PM_INFO. Происходит установка
 * ПМ в память.
 * */
static errcode_t create_pm(int8_t start_status){
	int16_t count;
	for(count = 0; count < sizeof(pm_pack) / sizeof(struct PM_INFO); count++){
		if(init_pm(&pm_pack[count], start_status) == NO_ERROR)
			print_debug("ПМ %s был успешно установлен\r\n", pm_pack[count].name);
		else{
			print_debug("ПМ %s не был установлен\r\n", pm_pack[count].name);
			return ERR_PM_INSTALL;
		}
	}
	return NO_ERROR;
}

