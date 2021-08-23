
SET SERVEROUTPUT ON; 

-- find_customer
CREATE OR REPLACE PROCEDURE 
find_customer(customer_id  IN NUMBER, found OUT NUMBER) AS
BEGIN
    SELECT COUNT(*)
    INTO found
    FROM customers
    WHERE cust_no = customer_id ;
EXCEPTION
    WHEN NO_DATA_FOUND THEN
        found := 0;
    WHEN OTHERS THEN
        DBMS_OUTPUT.PUT_LINE('Error');
END find_customer;
/

-- find_product
CREATE OR REPLACE PROCEDURE 
find_product (product_id IN NUMBER, price OUT products.prod_sell%TYPE) AS
BEGIN
    SELECT  prod_sell
    INTO    price
    FROM    products
    WHERE   prod_no = product_id;
EXCEPTION
    WHEN NO_DATA_FOUND THEN
        price := 0;
    WHEN OTHERS THEN
        DBMS_OUTPUT.PUT_LINE('Error');
END find_product;
/

-- add_order
CREATE OR REPLACE PROCEDURE 
add_order(customer_id IN NUMBER, new_order_id OUT NUMBER) AS
newDate orders.order_dt%TYPE;
BEGIN
    SELECT  MAX(order_no) + 1
    INTO    new_order_id
    FROM    orders;
    
    INSERT INTO orders
    (order_no,
    cust_no,
    status, 
    rep_no, 
    order_dt)
    VALUES
    (new_order_id,
    customer_id ,
    'C',
    02, 
    TO_CHAR(SYSDATE, 'DD-Mon-YYYY'));
EXCEPTION
    WHEN NO_DATA_FOUND THEN
        DBMS_OUTPUT.PUT_LINE('Error! No Data Found');
        DBMS_OUTPUT.PUT_LINE (SQLERRM);
        DBMS_OUTPUT.PUT_LINE (SQLCODE);
        new_order_id := 0;
    WHEN OTHERS THEN
        DBMS_OUTPUT.PUT_LINE('Error!');
        DBMS_OUTPUT.PUT_LINE (SQLERRM);
        DBMS_OUTPUT.PUT_LINE (SQLCODE);
END add_order;
/

-- add_orderline

CREATE OR REPLACE PROCEDURE
add_orderline (orderId IN orderlines.order_no%TYPE,
    itemId IN orderlines.line_no%TYPE,
    productId IN orderlines.prod_no%TYPE,
    quantity IN orderlines.qty%TYPE,
    price IN orderlines.price%TYPE) AS
BEGIN
INSERT INTO orderlines
    (order_no,
    line_no,
    prod_no,
    qty,
    price)
VALUES
    (orderId,
    itemId,
    productId,
    quantity,
    price);
EXCEPTION
    WHEN NO_DATA_FOUND THEN
    DBMS_OUTPUT.PUT_LINE('Error! No Data Found');
    DBMS_OUTPUT.PUT_LINE (SQLERRM);
    DBMS_OUTPUT.PUT_LINE (SQLCODE);
    WHEN OTHERS THEN
    DBMS_OUTPUT.PUT_LINE('Error!');
    DBMS_OUTPUT.PUT_LINE (SQLERRM);
    DBMS_OUTPUT.PUT_LINE (SQLCODE);
END add_orderline;
/
SELECT * 
    FROM orders 
    WHERE order_dt 
    LIKE '%2020%' 
    OR order_dt 
    LIKE '%2021%';


SELECT * 
    FROM orderlines ol 
    JOIN orders o 
    ON ol.order_no = o.order_no 
    WHERE o.order_dt 
    LIKE '%2020%' 
    OR o.order_dt 
    LIKE '%2021%';

ROLLBACK;
/