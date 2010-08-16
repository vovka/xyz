/*
 * Максимальное расстояние между вершинами прямоугольников, которое определяет
 * одинаковость прямоугольников, которым принадлежат эти вершины. То есть, если
 * взять два прямоугольника с упорядочеными вершинами (идущими одна за одной по
 * часовой стрелке, начиная с левой верхней) и если расстояние хотя бы между
 * двумя соответствующими вершинами меньше заданного, то прямоугольники
 * считаются одинаковыми.
 * Это сделано для того, чтобы исправить погрешность,
 * когда один прямоугольник с широкой границей распознаётся как множество
 * прямоугольников с границей в 1 пиксел.
 * Применятеся для прямоугольника внешней границы, для прямоугольников вопросов. 
 */
//const int SIMILAR_SQUARES_DISTANCE = 20;

/*
 * Минимальная площадь прямоугольников, которые могут быть чекбоксами
 */
//const int MIN_CHECKBOX_AREA = 100;

/*
 * То же, что и SIMILAR_SQUARES_DISTANCE, только применяется для чекбоксов.
 */
//const int SIMILAR_CHECKBOXES_DISTANCE = 8;


//const int MIN_VECTOR_LENGTH_FOR_SIMILARITY = 40;

/*
 * outer border width divided by question border width
 */
//const double QUESTION_TO_OUTER_RECT_WIDTH_RATIO = 1014. / 909.;

//const float CHECKBOXES_AREA_WIDTH_TO_QUESTION_WIDTH_RATIO = 0.1;

/*
 * расстояние между первой точкой знака выбора чекбокса (то, что ставит
 * пользователь - галочка, крестик и пр.) и первой (верхней левой) точкой
 * прямоугольника чекбокса, после которого считается, что знак выбора находится
 * в этом прямоугольнике чекбокса
 *
 * the distance between the first point of the sign choice checkbox (that puts
 * the user - a check mark, X, etc.) and the first (upper left) point of the
 * rectangle checkbox, after which it is believed that the sign of choice is in
 * the rectangle checkbox
 */
//const int MIN_VECTOR_LENGTH_FOR_SIMILARITY = 40;

/*
 * то же расстояние, но только по оси Y
 */
//const int MIN_Y_LENGTH_FOR_SIMILARITY = 10;

const int TEXT_ANSWER = -2;
const int NO_ANSWER = -1;
