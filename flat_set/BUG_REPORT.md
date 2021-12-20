## UB:
- в оптимизации STRONG создается вектор указателей, которые по ходу дела инвалидируются, вместо
		
		while (log_in.read((char*)&entry, sizeof(entry))) {
		   entries.push_back(entry);
		   entries_ptr.push_back(&entries.back());
		}
		
- должно быть

		while (log_in.read((char*)&entry, sizeof(entry))) {
		       entries.push_back(entry);
		}
		for (size_t i = 0; i < entries.size(); ++i) {
		       entries_ptr.push_back(&entries[i]);
		}
- (чтобы это понять, нужно было в корке увидеть STRONG уровень оптимизации, и что RE было в ApplyBatch)

## Потери производительности:

#### 1
- 43 строка journal, вместо 

		epoch != 0
- должно быть 

		last_snapshot_ != 0.
- (так как там по смыслу эта переменная)
- программа работает, но каждый раз выгружает всё из log

#### 2
- при считывании из журнала происходит вызов функции insert/erase, которая ... дописывает эту операцию в журнал. Чтобы это исправить я пока написал так: я в insert/erase добавил bool already_in_journal = false

## Потери данных

#### 1
-  в erase в set, он всегда удалял lower_bound, нужно вот это:
		
		if (found == keys_.end()) { return false }
- заменить на вот это (условие): 
		
		(found == keys_.end() || *found != value) {return false}

#### 2 (она же ошибка в логике)
- он рапортует в журнал о том, что добавил элемент до того, как реально его добавил, это приводит к потере поседнего добавленного элемента перед созданием snap_shot_

## Еще ошибки:

#### flush
- если завершать программу не через exit, то он не делает сброс буфера и данные теряются, чтобы этого избежать нужно добавить log_.flush() и out.flush() в нужных местах

#### Слишком сильная оптимизация
- в варианте оптимизации strong он должен был делать операции с маленьким set-om, а потом перемещал их в большой. Вот только, он сразу делал операции с больши сетом, оставляя маленький пустым.
- Если это испроавить, то он падает нга тестах, так как иногда нге делает erase, пришлось всю эту функцию переписать на:

		void ApplyBatch(std::vector<LogEntry<T>*>& entries) {
	        auto for_insert = FlatSet<T>();
	        auto for_erase = FlatSet<T>();

	        for (auto* entry : entries) {
	            if (entry->event == LogEvent::INSERT_KEY) {
	                for_insert.insert(entry->key);
	                for_erase.erase(entry->key);
	            } else {
	                for_insert.erase(entry->key);
	                for_erase.insert(entry->key);
	            }
	        }

	        for (const auto& key : for_erase.keys_) {
	            erase(key, true);
	        }
	        for (const auto& key : for_insert.keys_) {
	            insert(key, true);
	        }
	    }