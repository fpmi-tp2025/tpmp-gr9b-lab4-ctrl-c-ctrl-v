-- Test data for Parfum Bazaar application

-- Plain passwords: admin123 and makler123
INSERT INTO PERFUME_USERS (username, password_hash, role) VALUES
('admin', 'admin123', 'admin'),
('makler1', 'makler123', 'makler'),
('makler2', 'makler123', 'makler');

-- Insert test maklers
INSERT INTO PERFUME_MAKLERS (name, address, birth_year, user_id) VALUES
('Иванов Иван', 'ул. Московская, 10', 1980, 2),
('Петров Петр', 'ул. Ленина, 25', 1975, 3);

-- Insert test goods
INSERT INTO PERFUME_GOODS (name, type, unit_price, supplier, expiry_date, quantity) VALUES
('Chanel No 5', 'парфюмерия', 5000.00, 'Chanel France', '2025-12-31', 50),
('Dior Sauvage', 'парфюмерия', 4500.00, 'Dior Europe', '2025-10-15', 30),
('Tom Ford Black', 'парфюмерия', 6000.00, 'Tom Ford Inc', '2026-01-01', 20),
('L''Oreal Cream', 'косметика', 500.00, 'L''Oreal Russia', '2025-12-31', 100),
('MAC Lipstick', 'косметика', 1200.00, 'MAC Cosmetics', '2025-11-30', 80),
('Estee Lauder Set', 'косметика', 3000.00, 'Estee Lauder', '2026-06-30', 40),
('Givenchy Fragrance', 'парфюмерия', 4800.00, 'Givenchy Group', '2025-08-15', 25),
('Calvin Klein Eau', 'парфюмерия', 3500.00, 'Calvin Klein', '2025-09-30', 35),
('Lancome Collection', 'косметика', 2500.00, 'Lancome Paris', '2025-07-01', 60),
('Hugo Boss Cologne', 'парфюмерия', 3800.00, 'Hugo Boss', '2025-10-31', 45);

-- Insert test deals
INSERT INTO PERFUME_DEALS (deal_date, good_name, good_type, quantity, total_amount, makler_id, good_id, buyer) VALUES
('2024-05-01 10:30:00', 'Chanel No 5', 'парфюмерия', 5, 25000.00, 1, 1, 'Магазин "Люкс"'),
('2024-05-02 15:45:00', 'Dior Sauvage', 'парфюмерия', 3, 13500.00, 1, 2, 'Сеть "Парфюм"'),
('2024-05-03 11:20:00', 'L''Oreal Cream', 'косметика', 10, 5000.00, 2, 4, 'ООО "Красота"'),
('2024-05-03 16:00:00', 'MAC Lipstick', 'косметика', 8, 9600.00, 2, 5, 'Бутик "Элит"'),
('2024-05-04 09:15:00', 'Tom Ford Black', 'парфюмерия', 2, 12000.00, 1, 3, 'Магазин "Премиум"'),
('2024-05-04 14:30:00', 'Givenchy Fragrance', 'парфюмерия', 4, 19200.00, 2, 7, 'Сеть "Парфюм"'),
('2024-05-05 10:00:00', 'Estee Lauder Set', 'косметика', 6, 18000.00, 1, 6, 'ООО "Гламур"'),
('2024-05-05 17:45:00', 'Calvin Klein Eau', 'парфюмерия', 7, 24500.00, 2, 8, 'Магазин "Стиль"');

-- Initialize statistics for test data
INSERT INTO PERFUME_MAKLERSTATS (makler_id, good_name, good_type, total_quantity, total_amount)
SELECT makler_id, good_name, good_type, SUM(quantity), SUM(total_amount)
FROM PERFUME_DEALS
GROUP BY makler_id, good_name, good_type;
