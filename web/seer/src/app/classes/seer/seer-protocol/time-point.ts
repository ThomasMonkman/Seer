
/**
 * A point in time.
 * @export
 * @class TimePoint
 */
import { BasePoint } from 'app/classes/seer/seer-protocol/base-point';

export class TimePoint extends BasePoint {
    public readonly type: string = 'tp';
    public readonly name: string;
    public readonly thread: string;
    public readonly position: number;
    public readonly time: string;
    public readonly last: boolean;
    /**
     * Creates an instance of TimePoint.
     * @param {object} json the json to create from.
     * @memberof TimePoint
     */
    constructor(json: any) {
        super();
        this.name = json.n;
        this.thread = json.t;
        this.position = json.p;
        this.time = json.t;
        this.last = json.l;
    }
}
