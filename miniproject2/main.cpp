#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <semaphore.h>
#include <thread>
#include <time.h>
#include <unistd.h>
#include <vector>

class Cashier;
class Customer;

/// Тут в глобальную область выносятся
/// переменные, глобальные для программы:
/// количество продавцов и покупателей,
/// длительность рабочего дня, момент
/// времени начала работы, векторы
/// продавцов и покупателей, тред
/// для оповещения о конце рабочего дня.
size_t number_of_cashiers = 2;
size_t number_of_customers = 30;
time_t workday_duration = 10;
time_t begin;
std::vector<Cashier> cashiers;
std::vector<Customer> customers;
std::thread end_of_day;

/// Класс для покупателя - работает как поток, но умнее.
/// У каждого покупателя есть номер, семафор, тред для
/// реализации похода в магазин, номер очереди, в которой
/// он стоит и значение того, был ли он обслужен.
class Customer {
private:
    std::shared_ptr<sem_t> customer_semaphore;
    std::thread shopping;
    size_t id;
    size_t cashier_number;
    bool got_the_groceries = false;

public:
    /// Конструктор с параметрами номера покупателя -
    /// семафор генерируется дефолтно, номер кассы
    /// выбирается случайным образом.
    explicit Customer(size_t id) {
        this->id = id;
        customer_semaphore = std::make_shared<sem_t>();
        sem_init(customer_semaphore.get(), 0, 0);
        cashier_number = rand() % number_of_cashiers;
    }

    /// Геттер для идентификационного номера покупателя.
    size_t get_id() const { return id; }

    /// Метод для запуска треда с методом совершения покупок.
    void start() {
        shopping = std::thread(&Customer::shop, this);
    }

    /// Метод для того, чтобы джойнить приватный поток снаружи.
    void join() {
        shopping.join();
    }

    /// Метод уведомления - обслуживания с флагом того, обслужен
    /// ли покупатель. Флаг нужен для того, чтобы отделить случай
    /// удачного обслуживания от ситуации, когда покупатель
    /// пришел, когда магазин уже закрыт и ушел ни с чем.
    void notify(bool got_groceries) {
        this->got_the_groceries = got_groceries;
        sem_post(customer_semaphore.get());
    }

    /// Метод для реализации действий покупателя в очереди,
    /// определен в конце файла.
    void shop();
};

/// Класс кассира - работает как поток, но умнее.
/// У каждого кассира есть идентификационный номер,
/// семафор, очередь, которая к нему стоит,
/// мьютекс на эту очередь, тред и время начала работы.
class Cashier {
private:
    std::queue<Customer *> waiting_line;
    std::shared_ptr<std::mutex> line_mutex;
    std::shared_ptr<sem_t> cashier_semaphore;
    size_t id;
    std::thread working;
    time_t begin;

public:
    /// Конструктор для создания кассира с параметром
    /// идентификационного номера. Мьютекс и семафор
    /// создаются дефолтно, задается время начала работы.
    explicit Cashier(size_t id) {
        this->id = id;
        line_mutex = std::make_shared<std::mutex>();
        cashier_semaphore = std::make_shared<sem_t>();
        sem_init(cashier_semaphore.get(), 0, 0);
        time(&begin);
    }

    /// Метод для запуска работы треда с функцией работы.
    void start() {
        printf("\nCashier %zu starts their workday!\n", id);
        working = std::thread(&Cashier::work, this);
    }

    /// Метод для джойна треда снаружи.
    void join() {
        working.join();
    }

    /// Метод для уведомления кассира, например о приходе покупателя.
    void notify() {
        sem_post(cashier_semaphore.get());
    }

    /// Метод для того, чтобы покупатель добавился
    /// в очередь к кассиру - блокируется мьютекс очереди,
    /// туда добавляется покупатель, продавец уведомляется, мьютекс открывается.
    void join_line(Customer *new_customer) {
        line_mutex->lock();
        waiting_line.push(new_customer);
        notify();
        line_mutex->unlock();
    }

    /// Метод работы кассира - до тех пор, пока не закончился рабочий день,
    /// если в очереди пусто, кассир спит, иначе, если рабочий день кончился,
    /// все покупатели из очереди прогоняются, если рабочий день не кончился
    /// и в очереди не пусто, кассир обслуживает покупателя в начале очереди.
    /// Там есть несколько блоков сна, чтобы обслуживание не происходило
    /// моментально, а также чтобы покупатель уведомлялся об обслуживании
    /// раньше, чем продавец начинал обслуживать следующего.
    void work() {

        while (difftime(time(nullptr), begin) < workday_duration) {
            int val;
            sem_getvalue(cashier_semaphore.get(), &val);
            if (val == 0) {
                printf("\nThe %zu line is empty, so cashier %zu is sleeping\n", id, id);
            }
            sem_wait(cashier_semaphore.get());

            if (difftime(time(nullptr), begin) >= workday_duration) {
                line_mutex->lock();
                while (!waiting_line.empty()) {
                    waiting_line.front()->notify(false);
                    waiting_line.pop();
                }
                line_mutex->unlock();
                printf("\nCashier %zu is going home\n", id);
            } else if (!waiting_line.empty()) {
                line_mutex->lock();
                printf("\nCashier %zu is working with customer %zu...\n", id, waiting_line.front()->get_id());
                line_mutex->unlock();

                std::this_thread::sleep_for(std::chrono::milliseconds(50));

                line_mutex->lock();
                waiting_line.front()->notify(true);
                waiting_line.pop();
                line_mutex->unlock();

                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        }
    }
};

/// Метод для того, чтобы по истечение рабочего дня сообщить
/// об этом всем продавцам.
void end_of_day_alarm() {
    sleep(workday_duration);
    printf("\nTHE WORKDAY IS OVER!!!\n");
    for (auto &cashier : cashiers) {
        cashier.notify();
    }
}

/// Метод для считывания аргументов командной строки -
/// для каждого проверяется, парсится ли он в инт,
/// лежит ли он в допустимых пределах. В зависимости
/// от этого либо задаются дефолтные\минимальные\максимальные
/// значения, либо выставляются новые.
void parse_args(std::vector<std::string> args) {
    if (!args.empty()) {
        int new_number_of_cashiers;
        try {
            new_number_of_cashiers = std::stoi(args[0]);

            if (new_number_of_cashiers > 10) {
                printf("%d cashiers is a lot, "
                       "we'll run the simulation with 10\n", new_number_of_cashiers);
                number_of_cashiers = 10;
            } else if (new_number_of_cashiers < 1) {
                printf("%d cashiers is too little, "
                       "we'll run the simulation with 1\n", new_number_of_cashiers);
                number_of_cashiers = 1;
            } else {
                number_of_cashiers = new_number_of_cashiers;
            }
        } catch (std::invalid_argument) {
            printf("%s cashiers is a wrong thing, "
                   "we'll run the simulation with 2\n", args[0].c_str());
        }
    }
    if (args.size() > 1) {
        int new_number_of_customers;
        try {
            new_number_of_customers = std::stoi(args[1]);

            if (new_number_of_customers > 100) {
                printf("%d customers is a lot, "
                       "we'll run the simulation with 100\n", new_number_of_customers);
                number_of_customers = 100;
            } else if (new_number_of_customers < 1) {
                printf("%d customers is too little, "
                       "we'll run the simulation with 1\n", new_number_of_customers);
                number_of_customers = 1;
            } else {
                number_of_customers = new_number_of_customers;
            }
        } catch (std::invalid_argument) {
            printf("%s customers is a wrong thing, "
                   "we'll run the simulation with 30\n", args[1].c_str());
        }
    }
    if (args.size() > 2) {
        int new_workday_duration;
        try {
            new_workday_duration = std::stoi(args[2]);

            if (new_workday_duration > 30) {
                printf("%d seconds is a lot, "
                       "we'll run the simulation with 30 seconds\n", new_workday_duration);
                workday_duration = 30;
            } else if (new_workday_duration < 3) {
                printf("%d seconds is too little, "
                       "we'll run the simulation with 3 seconds\n", new_workday_duration);
                workday_duration = 3;
            } else {
                workday_duration = new_workday_duration;
            }
        } catch (std::invalid_argument) {
            printf("%s workday duration is a wrong thing, "
                   "we'll run the simulation with 10 seconds\n", args[2].c_str());
        }
    }

    printf("----------WE ARE SIMULATING A SHOP WITH %zu CASHIERS, "
           "%zu CUSTOMERS AND %ld SEC WORKDAY DURATION----------\n",
           number_of_cashiers, number_of_customers, workday_duration);
}

/// Метод для создания фигурирующих элементов - векторов
/// продавцов и покупателей, будильника на конец рабочего дня.
void make() {
    begin = time(nullptr);

    srand(begin);

    for (size_t i = 0; i < number_of_cashiers; ++i) {
        cashiers.emplace_back(i + 1);
    }

    end_of_day = std::thread(end_of_day_alarm);

    customers.reserve(number_of_customers);

    for (size_t i = 0; i < number_of_customers; ++i) {
        customers.emplace_back(i + 1);
    }
}

/// Для интересности покупатели приходят волнами, а не все сразу.
/// Об этом выводятся сообщения, запускается работа той или иной
/// группы покупателей.
void wave(std::string message, size_t lower_bound, size_t upper_bound, long sleeptime) {
    printf("\n-----------------------------------------------\n"
           "%s\n-----------------------------------------------\n",
           message.c_str());

    for (size_t i = lower_bound; i < upper_bound; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(sleeptime));
        customers[i].start();
    }
}

/// Метод для того, чтобы сджойнить все потоки.
void join() {
    end_of_day.join();

    for (size_t i = 0; i < number_of_cashiers; ++i) {
        cashiers[i].join();
    }

    for (size_t i = 0; i < number_of_customers; ++i) {
        customers[i].join();
    }
}

/// Главный метод приложения, вызывается метод для парсинга
/// аргументов командной строки, создания объектов, создания
/// первой волны покупателей, которые приходят раньше времени,
/// затем запускается работа кассиров, потом запускается вторая волна
/// покупателей, потом мы спим до двух секунд до конца рабочего дня
/// и запускаем третью волну покупателей, которые приходят за 2 секунды
/// до закрытия магазина. После этого джойним все потоки.
int main(int argc, char **argv) {
    std::vector<std::string> arguments(argv + 1, argv + argc);
    parse_args(arguments);

    make();

    wave("THE FIRST WAVE OF CUSTOMERS ARRIVES EARLY",
         0, number_of_customers / 3, 50);

    for (size_t i = 0; i < number_of_cashiers; ++i) {
        cashiers[i].start();
    }

    wave("THE SECOND WAVE OF CUSTOMERS ARRIVES AT NORMAL TIME",
         number_of_customers / 3, 2 * number_of_customers / 3, rand() % 300);

    sleep(begin + workday_duration - time(nullptr) - 2);

    wave("THE THIRD WAVE OF CUSTOMERS ARRIVES ALMOST LATE",
         2 * number_of_customers / 3, number_of_customers, rand() % 500);

    join();

    return 0;
}

/// Метод похода в магазин у покупателя. Если покупатель пришел до закрытия,
/// он становится в очередь к продавцу и ждет уведомления о своей очереди.
/// При получении уведомления он уходит, либо купив продукты, либо потому,
/// что магазин закрылся.
void Customer::shop() {
    if (begin + workday_duration - time(nullptr) > 0) {
        printf("\nCustomer %zu arrives!\nCustomer %zu is waiting in line and sleeping\n", id, id);
        cashiers[cashier_number].join_line(this);

        sem_wait(customer_semaphore.get());
    } else {
        printf("\nCustomer %zu arrives!", id);
    }

    printf("\nCustomer %zu is going home %s\n", id,
           got_the_groceries ? "having bought their groceries" : "because the shop closed");
}