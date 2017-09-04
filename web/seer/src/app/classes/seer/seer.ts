import { Observable } from 'rxjs/Rx';
import { BasePoint, TimePoint } from './seer-protocol';
/**
 * Parse and provides access to an seer connection.
 * @export
 * @class Seer
 */
export class Seer {
    private readonly source: Observable<BasePoint>;
    /**
     * Creates an instance of Seer.
     * @param {Observable<BasePoint>} source it should consume and parse.
     * @memberof Seer
     */
    constructor(source: Observable<BasePoint>) {
        this.source = source;
    }

    public timePoints(): Observable<TimePoint> {
        return this.source
            .share()
            .filter((point: BasePoint, i) => point.type === 'tp')
            .map(timePoint => <TimePoint> timePoint);
    }
}
